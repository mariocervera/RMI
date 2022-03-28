#ifndef __INCLUDE_METHODCALLER_H__
#define __INCLUDE_METHODCALLER_H__

#include "IRemoteMethod.h"
#include "Utils/DataStream.h"

namespace rmi {

  // This class encapsulates the invocation of a Remote Method on a Remote Object. The Remote Object is passed as a
  // parameter upon invocation.
  //
  // This class is implemented as a template. It allows clients to specify at compile time:
  //
  //    - T: the class where the encapsulated Method is implemented. It must be a subclass of IRemoteObject.
  //    - R: the return type of the encapsulated Method.
  //    - Args: the types of the arguments of the encapsulated Method.

  template <typename T, typename> class MethodCaller;

  template <typename T, typename R, typename... Args>
  class MethodCaller<T, R(Args...)> : public IRemoteMethod {

    public:
      using MethodPointer = R (T::*)(Args...);

      explicit MethodCaller(MethodPointer wrappedMethod) : wrappedMethod(wrappedMethod) { }

      // Deserializes the Byte Array of arguments prior to the invocation of the Method that is wrapped by this
      // Method Caller.
      ByteArray invoke(IRemoteObject& obj, ByteArray& args) override {

        std::tuple<std::decay_t<Args>...> argsTuple = deserializeByteArray(args);

        return doCall(*static_cast<T *>(&obj), argsTuple, std::is_void<R>());
      }

    private:
      // Deserializes data from a Byte Array and returns it as a Tuple. The Tuple establishes the specific types that
      // will be used for deserialization. These types correspond to the types of the input arguments of the method
      // wrapped by this Method Caller.
      std::tuple<std::decay_t<Args>...> deserializeByteArray(ByteArray& byteArray) {
        DataStream stream(byteArray);
        std::tuple<std::decay_t<Args>...> tuple;

        unrollDeserializationOperations(stream, tuple, std::index_sequence_for<Args...>{});

        return tuple;
      }

      // Unrolls the deserialization operations. There will be one operation per Tuple element.
      template <std::size_t... Is>
      void unrollDeserializationOperations(DataStream& stream,
                                           std::tuple<std::decay_t<Args>...>& tuple,
                                           std::index_sequence<Is...>) {

        std::initializer_list<int>({((stream >> std::get<Is>(tuple)), 0)...});
      }

      // Calls the Method encapsulated in this Method Caller upon the Remote Object. This is an overloaded function
      // that is resolved at compile time. It will only exist when R (the return type of the wrapped method) is void.
      // Since void methods do not return anything, this function does not require serialization of the result; therefore,
      // it returns an empty Byte Array.
      ByteArray doCall(T& remoteObject, std::tuple<std::decay_t<Args>...>& argsTuple, std::true_type RIsVoid) {

        invokeMethodPointer(remoteObject, argsTuple);
        return ByteArray();
      }

      // Calls the Method encapsulated in this Method Caller upon the Remote Object. This is an overloaded function that
      // is resolved at compile time. It will only exist when R (the return type of the wrapped method) is not void. Non-void
      // types require serialization of the result.
      ByteArray doCall(T& remoteObject, std::tuple<std::decay_t<Args>...>& argsTuple, std::false_type RIsVoid) {

        R result = invokeMethodPointer(remoteObject, argsTuple);
        return serialize<R>(result);
      }

      // Invokes the Method that is wrapped by this Method Caller. The invocation is performed upon a Remote Object. The
      // arguments are contained in a Tuple.
      R invokeMethodPointer(T& remoteObject, std::tuple<std::decay_t<Args>...>& argsTuple) {

        return invokeMethodPtr_unrollArguments(remoteObject, argsTuple, std::index_sequence_for<Args...>{});
      }

      // Invokes the Method that is wrapped by this Method Caller. The Tuple is unrolled to generate the method arguments that
      // are required for the call.
      template <std::size_t... Is>
      R invokeMethodPtr_unrollArguments(T& remoteObject, std::tuple<std::decay_t<Args>...>& argsTuple, std::index_sequence<Is...>) {

        return (remoteObject.*wrappedMethod)(std::get<Is>(argsTuple)...);
      }

    private:
      MethodPointer wrappedMethod;
  };

  template <typename E> static ByteArray serialize(E& element) {
    ByteArray byteArray;
    DataStream stream(byteArray);
    stream << element;

    return byteArray;
  }
}

#endif // __INCLUDE_METHODCALLER_H__
