#include "Utils/ByteArray.h"
#include "MethodCaller_TestFixtures.h"

using namespace testing;

namespace rmi {

  // Custom assertions.
  
  static void assertRemoteObjectGetsCorrectArguments(MultitypeSpy &remoteObject,
                                                     std::string &serializedString,
                                                     int serializedInteger) {
    ASSERT_EQ(remoteObject.getReceivedString(), serializedString);
    ASSERT_EQ(remoteObject.getReceivedInteger(), serializedInteger);
  }

  template <typename T>
  static void assertByteArrayContains(ByteArray &byteArray, T expectedValue) {

    DataStream stream(byteArray);
    T valueInByteArray;
    stream >> valueInByteArray;

    ASSERT_EQ(valueInByteArray, expectedValue);
  }

  
  // Parameterized tests.

  static void testMethodWithStringReferenceParameter(std::unique_ptr<IRemoteMethod> remoteMethod) {

    StringReferenceSpy remoteObject;
    ByteArray methodArgumentsArray;
    const std::string expectedString = serializeStringInto(methodArgumentsArray);

    remoteMethod->invoke(remoteObject, methodArgumentsArray);

    ASSERT_EQ(remoteObject.getReceivedString(), expectedString);
  }

  template <typename RemoteObjType>
  static void testMethodWithStringReferenceReturnType(std::unique_ptr<IRemoteMethod> remoteMethod) {

    RemoteObjType remoteObject;
    std::string expectedString = "Test String";
    remoteObject.setStringToReturn(expectedString);
    ByteArray emptyArguments;

    ByteArray resultByteArray = remoteMethod->invoke(remoteObject, emptyArguments);

    assertByteArrayContains<std::string>(resultByteArray, expectedString);
  }
  
  
  // Test methods.

  TEST_F(GivenMethodCallerOfVoidMethodWithNoArguments, WhenItIsInvokedThenReceiverObjectGetsTheMessage) {

    BooleanSpy remoteObject;
    ByteArray emptyArguments;

    remoteMethod->invoke(remoteObject, emptyArguments);

    ASSERT_TRUE(remoteObject.isCalled());
  }

  TEST_F(GivenMethodCallerOfVoidMethodWithNoArguments, WhenItIsInvokedThenItReturnsEmptyByteArray) {

    BooleanSpy remoteObject;
    ByteArray emptyArguments;

    const ByteArray byteArray = remoteMethod->invoke(remoteObject, emptyArguments);

    ASSERT_TRUE(byteArray.empty());
  }

  TEST_F(GivenMethodCallerOfVoidMethodWithTwoArguments, WhenItIsInvokedThenReceiverObjectGetsCorrectArguments) {

    MultitypeSpy remoteObject;
    ByteArray methodArgumentsArray;
    std::string arg1 = serializeStringInto(methodArgumentsArray);
    int arg2 = serializeIntegerInto(methodArgumentsArray);

    remoteMethod->invoke(remoteObject, methodArgumentsArray);

    assertRemoteObjectGetsCorrectArguments(remoteObject, arg1, arg2);
  }

  TEST_F(GivenMethodCallerOfIntegerMethodWithNoArguments, WhenItIsInvokedThenCorrectValueIsReturned) {

    int expectedReturnValue = 3;
    IntegerStub remoteObject(expectedReturnValue);
    ByteArray emptyArguments;

    ByteArray resultByteArray = remoteMethod->invoke(remoteObject, emptyArguments);

    assertByteArrayContains<int>(resultByteArray, expectedReturnValue);
  }

  TEST_F(GivenMethodCallerOfVoidMethodWithNonConstReferenceArgument, WhenItIsInvokedThenReceiverObjectGetsCorrectArgument) {

    testMethodWithStringReferenceParameter(std::move(remoteMethod));
  }

  TEST_F(GivenMethodCallerOfVoidMethodWithConstReferenceArgument, WhenItIsInvokedThenReceiverObjectGetsCorrectArgument) {

    testMethodWithStringReferenceParameter(std::move(remoteMethod));
  }

  TEST_F(GivenMethodCallerOfMethodThatReturnsStringReference, WhenItIsInvokedThenReceiverObjectReturnsCorrectValue) {

    testMethodWithStringReferenceReturnType<StringReferenceConfigurableStub>(std::move(remoteMethod));
  }

  TEST_F(GivenMethodCallerOfMethodThatReturnsStringConstReference, WhenItIsInvokedThenReceiverObjectReturnsCorrectValue) {

    testMethodWithStringReferenceReturnType<StringConstReferenceConfigurableStub>(std::move(remoteMethod));
  }

}
