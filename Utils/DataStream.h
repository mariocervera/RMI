#ifndef __INCLUDE_DATASTREAM_H__
#define __INCLUDE_DATASTREAM_H__

#include "ByteArray.h"

#define be32toh(x) (x)
#define htobe32(x) (x)

namespace rmi {

  // This class provides a serialization and deserialization interface that wraps a byte array.
  class DataStream {
    public:
      explicit DataStream(ByteArray & byteArray) : byteArray(byteArray), readIter(0) { }

      void readData(uint8_t *data, unsigned len) {
        byteArray.read(byteArray.begin() + readIter, data, len);
        readIter += len;
      }

      void writeData(const uint8_t *data, unsigned len) { byteArray.push_back(data, len); }

      DataStream &operator>>(uint32_t &i) {
        return this->operator>>(reinterpret_cast<int32_t &>(i));
  	  }

      DataStream &operator>>(int32_t &i) {
        readData(reinterpret_cast<uint8_t *>(&i), sizeof(i));
        i = be32toh(i);

        return *this;
      }

      DataStream &operator>>(std::string &value) {
        uint32_t length;
        this->operator>>(length);

        if (!length)
          return *this;

        value.resize(length);
        readData(reinterpret_cast<uint8_t *>(&*value.begin()), sizeof(uint8_t) * length);

        return *this;
      }

      DataStream &operator<<(int32_t i) {
        i = htobe32(i);
        writeData(reinterpret_cast<const uint8_t *>(&i), sizeof(i));

        return *this;
      }

      DataStream &operator<<(const std::string &value) {
      	this->operator<<(int32_t(value.size()));
      	writeData(reinterpret_cast<const uint8_t *>(value.data()), value.size());

      	return *this;
      }

    private:
      ByteArray& byteArray;
      unsigned readIter;
  };

}

#endif // __INCLUDE_DATASTREAM_H__
