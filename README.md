# RMI

The central class of this repository is the **MethodCaller** class, which has been developed using C++ template metaprogramming.

The *MethodCaller* class can be useful in the context of a framework for Remote Method Invocation (RMI). This class assumes that a RMI consists of bytes that are sent over a network from a client to a server.

The bytes of a RMI request encode:
   
   * *Remote method identifier*: a string that identifies the method to be invoked on a remote object that is on the server side.
   
   * *Remote object identifier*: a string that identifies the remote object upon which the remote method will be invoked.
   
   * *Arguments*: the data that will be passed into the remote method upon invocation.

One way to implement the method invocation on the remote object is through reflection. However, there is no reflection in C++.

A possible solution to this problem is to keep a map *M1* whose keys are object identifiers and whose values are remote objects. A map *M2* can store remote method identifiers as keys and objects conforming to a *RemoteMethod* interface as values.

Upong the reception of a RMI request, the server will extract the remote method and object identifiers. The object identifer allows the server to retrieve the receiver of the invocation from *M1*. The method idenfitier allows the server to retrieve from *M2* an instance of *RemoteMethod* and to call ...

```
ByteArray invoke(IRemoteObject& obj, ByteArray& args)
```

... passing the remote object and the remainder of the bytes from the RMI request as data.

The *invoke* method calls a preconfigured remote method on the remote object, effectively carrying out the RMI.

The *MethodCaller* class implements the *RemoteMethod* interface, and, therefore, it can be used to instantiate objects to be kept in *M2*.

### Example

The automated tests that are included in *MethodCallerTest.cpp* can serve as documentation for the *MethodCaller* class, but here's a short illustration of how it can be used.

Suppose we instantiate the MethodCaller class:

```
MethodCaller<Employee, void(int)> m;
```

When we call the *invoke* method on *m*, the *MethodCaller* will call a method on an *Employee* object. To make this possible, the *MethodCaller* object must receive a pointer to this method through its constructor.

The signature of the method that the *MethodCaller* "wraps" is, in this example: *void (int)*.

The *MethodCaller* class supports other types of method signatures, such as methods receiving several arguments or having reference parameters.

### Advantages

The key advantages of the *MethodCaller* class are:

   * It performs the required deserialization of arguments and serialization of results automatically. We must only instantiate a *MethodCaller* object with the right signature.
   
   * It has a simple and consistent interface that can be easily invoked when a RMI request is received on the server.



