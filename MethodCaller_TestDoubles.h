#ifndef __INCLUDE_METHODCALLER_TESTDOUBLES_H__
#define __INCLUDE_METHODCALLER_TESTDOUBLES_H__

#include "IRemoteObject.h"
#include <string>

namespace rmi {

  // A Spy implementation of an object to be invoked by a Method Caller. Tests can query these spies to find out
  // whether they have been called by the Method Caller or not.

  class BooleanSpy : public IRemoteObject {
    public:
      void call() { called = true; }
      bool isCalled() { return called; }

    private:
      bool called = false;
  };

  // A Spy implementation of an object to be invoked by a Method Caller. Tests can query these spies to find out
  // whether they have received (from the Method Caller) two different types of data: string and integer.

  class MultitypeSpy : public IRemoteObject {
    public:
      void call(std::string str, int number) {
        receivedString = str;
        receivedInteger = number;
      }

      std::string& getReceivedString() { return receivedString; }

      int getReceivedInteger() { return receivedInteger; }

    private:
      std::string receivedString = "a";
      int receivedInteger = 0;
  };

  // A Configurable Stub implementation of an object to be invoked by a Method Caller. Upon invocation, instances
  // of this class return an integer that is configured when the stub is instantiated.

  class IntegerStub : public IRemoteObject {
    public:
      explicit IntegerStub(int number) : value(number) { }

      int call() { return value; }

    private:
      int value = 0;
  };

  // A Spy implementation of an object to be invoked by a Method Caller. Tests can query these spies to find out
  // whether they have received correct data. The data is passed by the Method Caller as a string reference.

  class StringReferenceSpy : public IRemoteObject {
    public:
      void call(std::string& str) { receivedString = str; }
      void call(const std::string& str) { receivedString = str; }

      std::string& getReceivedString() { return receivedString; }

    private:
      std::string receivedString = "a";
  };

  // A Configurable Stub implementation of an object to be invoked by a Method Caller. Tests can configure these
  // stubs with a string that will be returned when the stub is called.

  class StringReferenceConfigurableStub : public IRemoteObject {
    public:
      void setStringToReturn(const std::string& str) { stringValue = str; }

      std::string& call() { return stringValue; }

    private:
      std::string stringValue = "";
  };

  // A Configurable Stub implementation of an object to be invoked by a Method Caller. Tests can configure these
  // stubs with a string that will be returned when the stub is called.

  class StringConstReferenceConfigurableStub : public IRemoteObject {
    public:
      void setStringToReturn(const std::string& str) { stringValue = str; }

      const std::string& call() { return stringValue; }

    private:
      std::string stringValue = "";
  };
}

#endif // __INCLUDE_METHODCALLER_TESTDOUBLES_H__
