#ifndef __INCLUDE_DATASTREAM_H__
#define __INCLUDE_DATASTREAM_H__

#include "ByteArray.h"
#include "Endian.h"
#include <string>
#include <vector>

#define le32toh(x) (x)
#define be32toh(x) (x)
#define htole32(x) (x)
#define htobe32(x) (x)

namespace elint
{

/// This class provides a serialization and deserialization interface that wraps
/// a byte array. User can configure the the internal byte ordering
/// representation.
class DataStream
{
  ByteArray &BA;
  const support::ByteOrder Order;
  unsigned ReadIter;

public:
  DataStream(ByteArray &BA, support::ByteOrder Order)
      : BA(BA), Order(Order), ReadIter(0)
  {
  }

  /// Reads len bytes from the stream and stores them into the input buffer.
  /// User must ensure that enough bytes are available for read. Byte ordering
  /// correction is not applied when using this function.
  void readData(uint8_t *Data, unsigned Len)
  {
    BA.read(BA.begin() + ReadIter, Data, Len);
    ReadIter += Len;
  }

  /// Writes len bytes into the stream from the input buffer. Byte ordering
  /// correction is not applied when using this function.
  void writeData(const uint8_t *Data, unsigned Len) { BA.push_back(Data, Len); }

  ///
  /// Read operators.
  ///
  
  DataStream &operator>>(uint32_t &i)
  {
    return this->operator>>(reinterpret_cast<int32_t &>(i));
  }

  DataStream &operator>>(int32_t &i)
  {
    readData(reinterpret_cast<uint8_t *>(&i), sizeof(i));
    if (Order == support::Order_LittleEndian)
      i = le32toh(i);
    else if (Order == support::Order_BigEndian)
      i = be32toh(i);
    return *this;
  }

  DataStream &operator>>(std::string &Value)
  {
    uint32_t Length;
    this->operator>>(Length);

    // If length is zero, stop reading bytes.
    if (!Length)
      return *this;

    Value.resize(Length);
    readData(reinterpret_cast<uint8_t *>(&*Value.begin()),
             sizeof(uint8_t) * Length);

    return *this;
  }

  ///
  /// Write operators.
  ///

  DataStream &operator<<(int32_t i)
  {
    if (Order == support::Order_LittleEndian)
      i = htole32(i);
    else if (Order == support::Order_BigEndian)
      i = htobe32(i);

    writeData(reinterpret_cast<const uint8_t *>(&i), sizeof(i));
    return *this;
  }

  DataStream &operator<<(const std::string &Value)
  {
    this->operator<<(int32_t(Value.size()));
    writeData(reinterpret_cast<const uint8_t *>(Value.data()), Value.size());
    return *this;
  }
};

} // namespace elint

#endif // __INCLUDE_DATASTREAM_H__
