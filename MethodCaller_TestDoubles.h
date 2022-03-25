#ifndef __INCLUDE_METHODCALLER_TESTDOUBLES_H__
#define __INCLUDE_METHODCALLER_TESTDOUBLES_H__

#include "IRemoteObject.h"
#include <string>

namespace rmi
{

/// A common superclass for all Remote Object Test Doubles. It avoids code
/// duplication by providing a dummy implementation of the clone method.
class RemoteObjectTestDouble : public IRemoteObject
{
public:
  std::unique_ptr<IRemoteObject>
  createNewInstance(ByteArray &Arguments) const override
  {
    return std::unique_ptr<RemoteObjectTestDouble>();
  }
};

/// A Spy implementation of an object to be invoked by a Method Caller. Tests
/// can perform boolean queries on these spies in order to find out whether
/// they have been called by the Method Caller or not.
class BooleanSpy : public RemoteObjectTestDouble
{
public:
  void call() { Called = true; }

  bool isCalled() { return Called; }

private:
  bool Called = false;
};

/// A Spy implementation of an object to be invoked by a Method Caller. Tests
/// can query these spies in order to find out whether they have received
/// (from the Method Caller) two different types of data: string and integer.
class MultitypeSpy : public RemoteObjectTestDouble
{
public:
  void call(std::string String, int Number)
  {
    ReceivedString = String;
    ReceivedInteger = Number;
  }

  std::string &getReceivedString() { return ReceivedString; }

  int getReceivedInteger() { return ReceivedInteger; }

private:
  std::string ReceivedString = "a";
  int ReceivedInteger = 0;
};

/// A Configurable Stub implementation of an object to be invoked by a Method
/// Caller. Upon invocation, instances of this class return an integer that can
/// be configured dynamically.
class IntegerStub : public RemoteObjectTestDouble
{
public:
  explicit IntegerStub(int Num) : Value(Num) {}

  int call() { return Value; }

private:
  int Value = 0;
};

/// A Spy implementation of an object to be invoked by a Method Caller. Tests
/// can query these spies in order to find out whether they have received
/// (from the Method Caller) the correct data, which is passed as a string
/// (const) reference.
class StringReferenceSpy : public RemoteObjectTestDouble
{
public:
  void call(std::string &String) { ReceivedString = String; }
  void call(const std::string &String) { ReceivedString = String; }

  std::string &getReceivedString() { return ReceivedString; }

private:
  std::string ReceivedString = "a";
};

/// A Configurable Stub implementation of an object to be invoked by a Method
/// Caller. Tests can configure these stubs with a string that will be returned
/// when the stub is called via RMI.
class StringReferenceConfigurableStub : public RemoteObjectTestDouble
{
public:
  void setStringToReturn(const std::string &String) { StringValue = String; }

  std::string &call() { return StringValue; }

private:
  std::string StringValue = "";
};

/// A Configurable Stub implementation of an object to be invoked by a Method
/// Caller. Tests can configure these stubs with a string that will be returned
/// when the stub is called via RMI.
class StringConstReferenceConfigurableStub : public RemoteObjectTestDouble
{
public:
  void setStringToReturn(const std::string &String) { StringValue = String; }

  const std::string &call() { return StringValue; }

private:
  std::string StringValue = "";
};

} // namespace rmi

#endif // __INCLUDE_METHODCALLER_TESTDOUBLES_H__
