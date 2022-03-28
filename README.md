# RMI

The central class of this repository is the *MethodCaller* class.

It has been developed using C++ template metaprogramming and it can be useful in the context of a framework for Remote Method Invocation (RMI).

The *MethodCaller* class assumes that a RMI consists of bytes that are sent over a network from a client to a server. These bytes encode:
   
   * Remote method identifier: a string that identifies the method to be invoked on a remote object that is on the server side.
   
   * Remote object identifier: a string that identifies the remote object upon which the remote method will be invoked.
   
   * Arguments: the data that will be passed into the remote method upon invocation.

One way to implement the method invocation on the remote object is through reflection. However, there is no reflection in C++.

A possible solution to this problem is to keep a map *M1* whose keys are remote object identifiers and the values are remote objects. A second map *M2* can store remote method identifiers as keys and objects conforming to a *RemoteMethod* interface as values.

An object conforming to the *RemoteMethod* interface can implement the following method:

```
ByteArray invoke(IRemoteObject& obj, ByteArray& args)
```

This method invokes a preconfigured remote method on a remote object from *M1*, receiving the arguments as a byte array and returning the result also as a byte array.

The *MethodCaller* class implements such an interface, and, therefore, it can be used to instantiate objects to be kept in *M2*.

### An example

The automated tests that are included in *MethodCallerTest.cpp* can serve as documentation for the *MethodCaller* class, but here's a short example for illustration purposes.

Suppose we instantiate the MethodCaller class:

```
MethodCaller<Employee, void(int)> m;
```

When we call the *invoke* method on *m*, the *MethodCaller* will invoke a method on an *Employee* object. The *MethodCaller*  object must, of course, receive a pointer to this method, whose signature, in this example, must be: void(int).

The *MethodCaller* class supports other types of method signatures, such as methods receiving several arguments or having reference parameters.

### Advantages

The key advantages of the *MethodCaller* class are:

   * It performs the required deserialization of arguments and serialization of results automatically, only instantiating an *MethodCaller* object with the right signature.
   
   * It has a simple and consistent interface that can be easily invoked when a RMI request is received on the server.



