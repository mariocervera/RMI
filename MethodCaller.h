#ifndef __INCLUDE_METHODCALLER_H__
#define __INCLUDE_METHODCALLER_H__

#include "IRemoteMethod.h"
#include "Utils/DataStream.h"

namespace elint
{

template <typename T, typename> class MethodCaller;

/// This class encapsulates the invocation of a Remote Method on a Remote
/// Object. This class is implemented as a variadic template. It allows clients
/// to specify at compile time:
/// 1) T: the class where the encapsulated Method is implemented. It must be a
/// subclass of IRemoteObject.
/// 2) R: the return type of the encapsulated Method.
/// 3) Args: the types of the arguments of the encapsulated Method.
/// Note: The Remote Object is passed dynamically as a parameter upon
/// invocation.
template <typename T, typename R, typename... Args>
class MethodCaller<T, R(Args...)> : public IRemoteMethod
{
public:
  using MethodPointer = R (T::*)(Args...);

  explicit MethodCaller(MethodPointer WrappedMethod)
      : WrappedMethod(WrappedMethod)
  {
  }

  /// Deserializes the Byte Array of arguments prior to the invocation of the
  /// Method that is wrapped by this Method Caller.
  ByteArray invoke(IRemoteObject &Obj, ByteArray &Arguments) override
  {
    std::tuple<std::decay_t<Args>...> ArgsTuple =
        deserializeByteArray(Arguments);

    return doCall(*static_cast<T *>(&Obj), ArgsTuple, std::is_void<R>());
  }

private:
  /// Deserializes data from a Byte Array and returns it as a Tuple. The Tuple
  /// establishes the specific types that will be used for deserialization.
  /// These types correspond to the types of the input arguments of the method
  /// wrapped by this Method Caller.
  std::tuple<std::decay_t<Args>...> deserializeByteArray(ByteArray &Array)
  {
    DataStream Stream(Array, support::Order_BigEndian);
    std::tuple<std::decay_t<Args>...> Tuple;

    unrollDeserializationOperations(Stream, Tuple,
                                    std::index_sequence_for<Args...>{});

    return Tuple;
  }

  /// Unrolls the deserialization operations. There will be one operation per
  /// Tuple element.
  template <std::size_t... Is>
  void unrollDeserializationOperations(DataStream &Stream,
                                       std::tuple<std::decay_t<Args>...> &Tuple,
                                       std::index_sequence<Is...>)
  {
    std::initializer_list<int>({((Stream >> std::get<Is>(Tuple)), 0)...});
  }

  /// Calls the Method encapsulated in this Method Caller upon the Remote
  /// Object. This is an overloaded function that is resolved at compile time.
  /// It will only exist when R (the return type of the wrapped method) is void.
  /// Since void methods do not return anything, this function does not require
  /// serialization of the result; therefore, it returns an empty Byte Array.
  ByteArray doCall(T &RemoteObject,
                   std::tuple<std::decay_t<Args>...> &ArgsTuple,
                   std::true_type RIsVoid)
  {
    invokeMethodPointer(RemoteObject, ArgsTuple);

    return ByteArray();
  }

  /// Calls the Method encapsulated in this Method Caller upon the Remote
  /// Object. This is an overloaded function that is resolved at compile time.
  /// It will only exist when R (the return type of the wrapped method) is not
  /// void. Non-void types require serialization of the result.
  ByteArray doCall(T &RemoteObject,
                   std::tuple<std::decay_t<Args>...> &ArgsTuple,
                   std::false_type RIsVoid)
  {
    R Result = invokeMethodPointer(RemoteObject, ArgsTuple);

    return serialize<R>(Result);
  }

  /// Invokes the Method that is wrapped by this Method Caller. The invocation
  /// is performed upon a Remote Object and the arguments are collected in
  /// a Tuple.
  R invokeMethodPointer(T &RemoteObject,
                        std::tuple<std::decay_t<Args>...> &ArgsTuple)
  {
    return invokeMethodPtr_unrollArguments(RemoteObject, ArgsTuple,
                                           std::index_sequence_for<Args...>{});
  }

  /// Invokes the Method that is wrapped by this Method Caller. The Tuple is
  /// unrolled to generate the method arguments that are required for the call.
  template <std::size_t... Is>
  R invokeMethodPtr_unrollArguments(
      T &RemoteObject, std::tuple<std::decay_t<Args>...> &ArgsTuple,
      std::index_sequence<Is...>)
  {
    return (RemoteObject.*WrappedMethod)(std::get<Is>(ArgsTuple)...);
  }

  /// Serializes the input parameter into a Byte Array.
  template <typename E> static ByteArray serialize(E &Element)
  {
    ByteArray BA;
    DataStream Stream(BA, support::Order_BigEndian);
    Stream << Element;

    return BA;
  }

private:
  MethodPointer WrappedMethod;
};

} // namespace elint

#endif // __INCLUDE_METHODCALLER_H__
