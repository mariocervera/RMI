#ifndef __INCLUDE_IREMOTEMETHOD_H__
#define __INCLUDE_IREMOTEMETHOD_H__

#include "Utils/ByteArray.h"

namespace elint
{

class IRemoteObject;

/// An abstract representation of a Method that can be invoked on a Remote
/// Object.
class IRemoteMethod
{
public:
  virtual ~IRemoteMethod() = default;

  /// Invokes this Method on the given Remote Object passing the Arguments as a
  /// Byte Array. The Method response is also encoded as a Byte Array.
  virtual ByteArray invoke(IRemoteObject &Obj, ByteArray &Arguments) = 0;
};

} // namespace elint

#endif // __INCLUDE_IREMOTEMETHOD_H__
