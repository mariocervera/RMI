#ifndef __INCLUDE_IREMOTEOBJECT_H__
#define __INCLUDE_IREMOTEOBJECT_H__

#include "Utils/ByteArray.h"

namespace rmi {

	// An abstract representation of an object that can be invoked remotely.
	class IRemoteObject {
		public:
		  virtual ~IRemoteObject() = default;
	};
}

#endif // __INCLUDE_IREMOTEOBJECT_H__
