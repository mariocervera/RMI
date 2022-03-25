#ifndef __INCLUDE_MATHEXTRAS_H__
#define __INCLUDE_MATHEXTRAS_H__

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>

namespace elint
{

/// This function takes a 64-bit integer and returns the bit equivalent double.
inline double bitsToDouble(uint64_t Bits)
{
  double D;
  std::memcpy(&D, &Bits, sizeof(Bits));
  return D;
}

/// This function takes a 32-bit integer and returns the bit equivalent float.
inline float bitsToFloat(uint32_t Bits)
{
  float F;
  std::memcpy(&F, &Bits, sizeof(Bits));
  return F;
}

/// This function takes a double and returns the bit equivalent 64-bit integer.
inline uint64_t doubleToBits(double Double)
{
  uint64_t Bits;
  std::memcpy(&Bits, &Double, sizeof(Double));
  return Bits;
}

/// This function takes a float and returns the bit equivalent 32-bit integer.
inline uint32_t floatToBits(float Float)
{
  uint32_t Bits;
  std::memcpy(&Bits, &Float, sizeof(Float));
  return Bits;
}

/// Converts from float16 to float32.
inline float float16to32(const uint16_t Half)
{
  uint32_t NonSign = ((Half & 0x7fff) << 13) +
                     0x38000000;         // Non sign aligned and bias adjusted
  uint32_t Sign = (Half & 0x8000) << 16; // Sign aligned
  uint32_t Exp = Half & 0x7c00;          // Exponent

  NonSign = (Exp == 0) ? 0 : NonSign; // Exp = 0
  NonSign |= Sign;                    // Insert Sign

  float Result = 0.0f;                           // RVO
  std::memcpy(&Result, &NonSign, sizeof(float)); // uint32_t to float
  return Result;
}

/// Converts from float32 to float16.
inline uint16_t float32to16(const float FullF)
{
  uint32_t Full;
  std::memcpy(&Full, &FullF, sizeof(float)); // float to uint32_t

  uint32_t NonSign = ((Full & 0x7FFFFFFF) >> 13) -
                     0x1C000; // Non sign aligned and bias adjusted
  uint32_t Sign = (Full & 0x80000000) >> 16; // Sign aligned
  uint32_t Exp = Full & 0x7F800000;          // Exponent

  NonSign = (Exp < 0x38800000) ? 0 : NonSign;      // Flush to Zero
  NonSign = (Exp > 0x47000000) ? 0x7BFF : NonSign; // Clamp to maximum
  NonSign |= Sign;                                 // Insert Sign

  return static_cast<uint16_t>(NonSign & 0xFFFF);
}

/// Sign extend B-bit number x to 32-bit int.
template <unsigned B> inline int32_t signExtend32(uint32_t x)
{
  return int32_t(x << (32 - B)) >> (32 - B);
}

/// This function returns the floor log base 2 of the specified value, 0 if the
/// value is 0.
//inline unsigned Log2_32(uint32_t Value) { return 31 - __builtin_clz(Value); }

/// Convert an integer with B bits at the right of the point into a floating
/// point value.
template <unsigned B, typename T> inline float fixedPointToFloat(T x)
{
  return x * (1.f / (1u << B));
}

/// Normalizes the given value the maximum value range of T.
template <typename T> inline float normalize(T Value)
{
  return float(Value) / std::numeric_limits<T>::max();
}

/// Denormalizes the given value the maximum value range of T.
template <typename T> inline float denormalize(T Value)
{
  return float(Value) * std::numeric_limits<T>::max();
}

/// Returns true when a value is within a given range, otherwise false.
template <typename T> inline bool isInRange(T Value, T Min, T Max)
{
  return (Value >= Min && Value <= Max);
}

/// Swaps the endianness between two values.
template <typename T>
T swapEndian(const T &Val,
             typename std::enable_if<std::is_arithmetic<T>::value,
                                     std::nullptr_t>::type = nullptr)
{
  union U {
    T Val;
    std::array<uint8_t, sizeof(T)> Raw;
  } Src, Dst;

  Src.Val = Val;
  std::reverse_copy(Src.Raw.begin(), Src.Raw.end(), Dst.Raw.begin());
  return Dst.Val;
}

} // namespace elint

#endif //__INCLUDE_MATHEXTRAS_H__
