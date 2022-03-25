#ifndef __INCLUDE_IREMOTEOBJECT_H__
#define __INCLUDE_IREMOTEOBJECT_H__

#include "Utils/ByteArray.h"

namespace rmi
{

/// An abstract representation of a Method that can be invoked on a Remote
/// Object.
class IRemoteObject
{
public:
  virtual ~IRemoteObject() = default;

  virtual std::unique_ptr<IRemoteObject> createNewInstance(ByteArray& Arguments) const = 0;
};

} // namespace rmi

#endif // __INCLUDE_IREMOTEOBJECT_H__
