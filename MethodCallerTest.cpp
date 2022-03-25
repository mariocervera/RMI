#include "Utils/ByteArray.h"
#include "MethodCaller_TestFixtures.h"

using namespace testing;

namespace rmi
{

/// Asserts the remote object receives the correct arguments (of type string and
/// int).
static void
assertRemoteObjectGetsCorrectArguments(MultitypeSpy &RemoteObject,
                                       std::string &SerializedString,
                                       int SerializedInteger)
{
  ASSERT_EQ(RemoteObject.getReceivedString(), SerializedString);
  ASSERT_EQ(RemoteObject.getReceivedInteger(), SerializedInteger);
}

/// Checks whether a Byte Array contains an expected value.
template <typename T>
static void assertByteArrayContains(ByteArray &ByteArr, T ExpectedValue)
{
  DataStream Stream(ByteArr);
  T ValueInByteArray;
  Stream >> ValueInByteArray;

  ASSERT_EQ(ValueInByteArray, ExpectedValue);
}

/// A parameterized test for a Method Caller that wraps a method with one
/// string parameter, which is passed by reference. The test checks that the
/// method receives the correct string.
static void testMethodWithStringReferenceParameter(
    std::unique_ptr<IRemoteMethod> RemoteMethod)
{
  StringReferenceSpy RemoteObject;
  ByteArray MethodArgumentsArray;
  std::string ExpectedString = serializeStringInto(MethodArgumentsArray);

  RemoteMethod->invoke(RemoteObject, MethodArgumentsArray);

  ASSERT_EQ(RemoteObject.getReceivedString(), ExpectedString);
}

/// A parameterized test for a Method Caller that wraps a method with no
/// arguments and string reference return type. The test checks that the Method
/// Caller returns the correct string.
template <typename RemoteObjType>
static void testMethodWithStringReferenceReturnType(
    std::unique_ptr<IRemoteMethod> RemoteMethod)
{
  RemoteObjType RemoteObject;
  std::string ExpectedString = "Test String";
  RemoteObject.setStringToReturn(ExpectedString);
  ByteArray EmptyArguments;

  ByteArray ResultByteArray =
      RemoteMethod->invoke(RemoteObject, EmptyArguments);

  assertByteArrayContains<std::string>(ResultByteArray, ExpectedString);
}

TEST_F(GivenMethodCallerOfVoidMethodWithNoArguments,
       WhenItIsInvokedThenReceiverObjectGetsTheMessage)
{
  BooleanSpy RemoteObject;
  ByteArray EmptyArguments;

  RemoteMethod->invoke(RemoteObject, EmptyArguments);

  ASSERT_TRUE(RemoteObject.isCalled());
}

TEST_F(GivenMethodCallerOfVoidMethodWithNoArguments,
       WhenItIsInvokedThenItReturnsEmptyByteArray)
{
  BooleanSpy RemoteObject;
  ByteArray EmptyArguments;

  ByteArray ByteArr = RemoteMethod->invoke(RemoteObject, EmptyArguments);

  ASSERT_TRUE(ByteArr.size() == 0);
}

TEST_F(GivenMethodCallerOfVoidMethodWithTwoArguments,
       WhenItIsInvokedThenReceiverObjectGetsCorrectArguments)
{
  MultitypeSpy RemoteObject;
  ByteArray MethodArgumentsArray;
  std::string Arg1 = serializeStringInto(MethodArgumentsArray);
  int Arg2 = serializeIntegerInto(MethodArgumentsArray);

  RemoteMethod->invoke(RemoteObject, MethodArgumentsArray);

  assertRemoteObjectGetsCorrectArguments(RemoteObject, Arg1, Arg2);
}

TEST_F(GivenMethodCallerOfIntegerMethodWithNoArguments,
       WhenItIsInvokedThenCorrectValueIsReturned)
{
  int ExpectedReturnValue = 3;
  IntegerStub RemoteObject(ExpectedReturnValue);
  ByteArray EmptyArguments;

  ByteArray ResultByteArray =
      RemoteMethod->invoke(RemoteObject, EmptyArguments);

  assertByteArrayContains<int>(ResultByteArray, ExpectedReturnValue);
}

TEST_F(GivenMethodCallerOfVoidMethodWithNonConstReferenceArgument,
       WhenItIsInvokedThenReceiverObjectGetsCorrectArgument)
{
  testMethodWithStringReferenceParameter(std::move(RemoteMethod));
}

TEST_F(GivenMethodCallerOfVoidMethodWithConstReferenceArgument,
       WhenItIsInvokedThenReceiverObjectGetsCorrectArgument)
{

  testMethodWithStringReferenceParameter(std::move(RemoteMethod));
}

TEST_F(GivenMethodCallerOfMethodThatReturnsStringReference,
       WhenItIsInvokedThenReceiverObjectReturnsCorrectValue)
{
  testMethodWithStringReferenceReturnType<StringReferenceConfigurableStub>(
      std::move(RemoteMethod));
}

TEST_F(GivenMethodCallerOfMethodThatReturnsStringConstReference,
       WhenItIsInvokedThenReceiverObjectReturnsCorrectValue)
{
  testMethodWithStringReferenceReturnType<StringConstReferenceConfigurableStub>(
      std::move(RemoteMethod));
}

} // namespace rmi
