#ifndef __INCLUDE_METHODCALLER_TESTFIXTURES_H__
#define __INCLUDE_METHODCALLER_TESTFIXTURES_H__

#include "MethodCaller.h"
#include "Utils/DataStream.h"
#include "MethodCaller_TestDoubles.h"
#include <gtest/gtest.h>

namespace rmi {

  template <typename T> void serializeInto(ByteArray &byteArray, T value) {
    DataStream stream(byteArray);
    stream << value;
  }

  std::string serializeStringInto(ByteArray &byteArray) {
    std::string testString = "TestString";
    serializeInto<std::string>(byteArray, testString);

    return testString;
  }

  int serializeIntegerInto(ByteArray & byteArray) {
    int testValue = 2;
    serializeInto<int>(byteArray, testValue);

    return testValue;
  }


  // Fixture superclass for testing the Method Caller.

  template <typename T, typename> class GivenAMethodCaller;

  template <typename T, typename R, typename... Args>
  class GivenAMethodCaller<T, R(Args...)> : public ::testing::Test {
    
  protected:
    using MethodCallerType = MethodCaller<T, R(Args...)>;

    void SetUp() override {
      R(T:: *M)(Args...) = &T::call;
      remoteMethod = std::make_unique<MethodCallerType>(M);
    }

  protected:
    std::unique_ptr<IRemoteMethod> remoteMethod;
  };

  
  // A fixture for testing a Method Caller that wraps a method with void return type and no arguments.

  class GivenMethodCallerOfVoidMethodWithNoArguments
    : public GivenAMethodCaller<BooleanSpy, void()> { };


  // A fixture for testing a Method Caller that wraps a method with void return type and two arguments (of type string and integer).

  class GivenMethodCallerOfVoidMethodWithTwoArguments
    : public GivenAMethodCaller<MultitypeSpy, void(std::string, int)> { };


  // A fixture for testing a Method Caller that wraps a method with integer return type and no arguments.

  class GivenMethodCallerOfIntegerMethodWithNoArguments
    : public GivenAMethodCaller<IntegerStub, int()> { };

  
  // A fixture for testing a Method Caller that wraps a method with void return type and one argument (of type string reference).

  class GivenMethodCallerOfVoidMethodWithNonConstReferenceArgument
    : public GivenAMethodCaller<StringReferenceSpy, void(std::string &)> { };


  // A fixture for testing a Method Caller that wraps a method with void return type and one argument (of type string const reference).

  class GivenMethodCallerOfVoidMethodWithConstReferenceArgument
    : public GivenAMethodCaller<StringReferenceSpy, void(const std::string &)> { };


  // A fixture for testing a Method Caller that wraps a method that returns a string reference.

  class GivenMethodCallerOfMethodThatReturnsStringReference
    : public GivenAMethodCaller<StringReferenceConfigurableStub, std::string &()> { };


  // A fixture for testing a Method Caller that wraps a method that returns a string const reference.

  class GivenMethodCallerOfMethodThatReturnsStringConstReference
    : public GivenAMethodCaller<StringConstReferenceConfigurableStub, const std::string &()> { };

}

#endif // __INCLUDE_METHODCALLER_TESTFIXTURES_H__
