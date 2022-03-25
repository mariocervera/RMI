#ifndef __INCLUDE_DATASTREAM_H__
#define __INCLUDE_DATASTREAM_H__

#include "ArgumentPack.h"
#include "ByteArray.h"
#include "Endian.h"
#include "MathExtras.h"
#include <string>
#include <vector>

#define le16toh(x) (x)
#define le32toh(x) (x)
#define le64toh(x) (x)
#define htole16(x) (x)
#define htole32(x) (x)
#define htole64(x) (x)
#define be16toh(x) (x)
#define htobe32(x) (x)
#define be32toh(x) (x)
#define be64toh(x) (x)
#define htobe16(x) (x)
#define htobe64(x) (x)

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
  DataStream(const DataStream &) = delete;
  DataStream &operator=(const DataStream &) = delete;

  DataStream(ByteArray &BA, support::ByteOrder Order)
      : BA(BA), Order(Order), ReadIter(0)
  {
  }

  /// Returns the current byte ordering setting.
  support::ByteOrder byteOrder() const { return Order; }

  /// Returns the underlying byte array of this stream.
  ByteArray &array() { return BA; }
  const ByteArray &array() const { return BA; }

  /// Reads len bytes from the stream and stores them into the input buffer.
  /// User must ensure that enough bytes are available for read. Byte ordering
  /// correction is not applied when using this function.
  void readData(uint8_t *Data, unsigned Len)
  {
    BA.read(BA.begin() + ReadIter, Data, Len);
    ReadIter += Len;
  }

  /// Reads the remaining bytes from the stream and stores them into the input
  /// buffer. User must ensure that bytes are available for read. Byte ordering
  /// correction is not applied when using this function.
  void readRemainingData(uint8_t *Data)
  {
    unsigned Size = BA.size();
    BA.read(BA.begin() + ReadIter, Data, Size - ReadIter);
    ReadIter = Size;
  }

  /// Reads the remaining bytes from the stream and stores them into the input
  /// buffer. The buffer will be resized to store the remaining data. Byte
  /// ordering correction is not applied when using this function.
  void readRemainingData(ByteArray &Data)
  {
    unsigned Size = BA.size();
    unsigned RemainingData = Size - ReadIter;

    if (!RemainingData)
      return;

    Data.resize(RemainingData);

    BA.read(BA.begin() + ReadIter, Data.begin(), RemainingData);
    ReadIter = Size;
  }

  /// Skips len bytes from the byte array.
  void skip(unsigned Len) { ReadIter += Len; }

  /// Writes len bytes into the stream from the input buffer. Byte ordering
  /// correction is not applied when using this function.
  void writeData(const uint8_t *Data, unsigned Len) { BA.push_back(Data, Len); }

  ///
  /// Read operators.
  ///
  DataStream &operator>>(bool &i)
  {
    return this->operator>>(reinterpret_cast<int8_t &>(i));
  }
  DataStream &operator>>(uint8_t &i)
  {
    return this->operator>>(reinterpret_cast<int8_t &>(i));
  }
  DataStream &operator>>(int8_t &i)
  {
    readData(reinterpret_cast<uint8_t *>(&i), sizeof(i));
    return *this;
  }
  DataStream &operator>>(uint16_t &i)
  {
    return this->operator>>(reinterpret_cast<int16_t &>(i));
  }
  DataStream &operator>>(int16_t &i)
  {
    readData(reinterpret_cast<uint8_t *>(&i), sizeof(i));
    if (Order == support::Order_LittleEndian)
      i = le16toh(i);
    else if (Order == support::Order_BigEndian)
      i = be16toh(i);
    else
      assert(0 && "Unsupported byte order");
    return *this;
  }
  DataStream &operator>>(uint32_t &i)
  {
    return this->operator>>(reinterpret_cast<int32_t &>(i));
  }
  DataStream &operator>>(float &i)
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
    else
      assert(0 && "Unsupported byte order");
    return *this;
  }
  DataStream &operator>>(uint64_t &i)
  {
    return this->operator>>(reinterpret_cast<int64_t &>(i));
  }
  DataStream &operator>>(double &i)
  {
    return this->operator>>(reinterpret_cast<int64_t &>(i));
  }
  DataStream &operator>>(int64_t &i)
  {
    readData(reinterpret_cast<uint8_t *>(&i), sizeof(i));
    if (Order == support::Order_LittleEndian)
      i = le64toh(i);
    else if (Order == support::Order_BigEndian)
      i = be64toh(i);
    else
      assert(0 && "Unsupported byte order");
    return *this;
  }
  DataStream &operator>>(std::string &Value)
  {
    static_assert(sizeof(char) == 1 && "Char size must be 1 Byte");

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
  DataStream &operator<<(bool b) { return this->operator<<(int8_t(b)); }
  DataStream &operator<<(uint8_t i) { return this->operator<<(int8_t(i)); }
  DataStream &operator<<(int8_t i)
  {
    writeData(reinterpret_cast<const uint8_t *>(&i), sizeof(i));
    return *this;
  }
  DataStream &operator<<(uint16_t i) { return this->operator<<(int16_t(i)); }
  DataStream &operator<<(int16_t i)
  {
    if (Order == support::Order_LittleEndian)
      i = htole16(i);
    else if (Order == support::Order_BigEndian)
      i = htobe16(i);
    else
      assert(0 && "Unsupported byte order");
    writeData(reinterpret_cast<const uint8_t *>(&i), sizeof(i));
    return *this;
  }
  DataStream &operator<<(uint32_t i) { return this->operator<<(int32_t(i)); }
  DataStream &operator<<(float f)
  {
    return this->operator<<(int32_t(floatToBits(f)));
  }
  DataStream &operator<<(int32_t i)
  {
    if (Order == support::Order_LittleEndian)
      i = htole32(i);
    else if (Order == support::Order_BigEndian)
      i = htobe32(i);
    else
      assert(0 && "Unsupported byte order");
    writeData(reinterpret_cast<const uint8_t *>(&i), sizeof(i));
    return *this;
  }
  DataStream &operator<<(uint64_t i) { return this->operator<<(int64_t(i)); }
  DataStream &operator<<(double d)
  {
    return this->operator<<(int64_t(doubleToBits(d)));
  }
  DataStream &operator<<(int64_t i)
  {
    if (Order == support::Order_LittleEndian)
      i = htole64(i);
    else if (Order == support::Order_BigEndian)
      i = htobe64(i);
    else
      assert(0 && "Unsupported byte order");
    writeData(reinterpret_cast<const uint8_t *>(&i), sizeof(i));
    return *this;
  }
  DataStream &operator<<(const ByteArray &BA)
  {
    writeData(BA.data(), BA.size());
    return *this;
  }
  DataStream &operator<<(const std::string &Value)
  {
    static_assert(sizeof(char) == 1 && "Char size must be 1 Byte");

    this->operator<<(int32_t(Value.size()));
    writeData(reinterpret_cast<const uint8_t *>(Value.data()), Value.size());
    return *this;
  }
};

/// NOTE: T must implement << operator so this function can work properly.
template <typename T>
inline DataStream &operator<<(DataStream &D, const std::vector<T> &V)
{
  D << uint32_t(V.size());
  for (const auto &i : V)
    D << i;

  return D;
}

/// NOTE: T must implement >> operator so this function can work properly.
template <typename T>
inline DataStream &operator>>(DataStream &D, std::vector<T> &V)
{
  uint32_t Size;
  D >> Size;
  for (unsigned i = 0; i != Size; ++i)
  {
    T A;
    D >> A;
    V.push_back(A);
  }

  return D;
}

/// NOTE: T and R must implement << operator so this function can work properly.
template <typename T, typename R>
inline DataStream &operator<<(DataStream &D, const std::pair<T, R> &P)
{
  D << P.first << P.second;

  return D;
}

/// NOTE: T and R must implement >> operator so this function can work properly.
template <typename T, typename R>
inline DataStream &operator>>(DataStream &D, std::pair<T, R> &P)
{
  D >> P.first >> P.second;

  return D;
}

inline DataStream &operator<<(DataStream &Stream, const ArgumentPack &Props)
{
  Stream << uint32_t(Props.size());

  for (const auto &Property : Props)
    Stream << Property.first << Property.second;

  return Stream;
}

inline DataStream &operator>>(DataStream &Stream, ArgumentPack &Props)
{
  uint32_t ElementCount;

  Stream >> ElementCount;

  std::string Key, Value;
  for (unsigned i = 0; i != ElementCount; ++i)
  {
    Key.clear();
    Value.clear();
    Stream >> Key >> Value;
    Props.insert(Key, Value);
  }

  return Stream;
}

} // namespace elint

#endif // __INCLUDE_DATASTREAM_H__
