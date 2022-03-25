#ifndef __INCLUDE_BYTEARRAY_H__
#define __INCLUDE_BYTEARRAY_H__

#include <fstream>

namespace rmi
{

/// This class provides a dynamic array of bytes. It grows as new
/// elements are inserted, similar to std::vector.
class ByteArray
{
  enum
  {
    InitialBufferSize = 512u,
    MaximumBufferSize = 4u * 1024u * 1024u
  };

public:
  using value_type = uint8_t;
  using iterator = value_type *;
  using const_iterator = const value_type *;

  ByteArray(const ByteArray &Other)
      : Capacity(Other.Capacity), Size(Other.Size)
  {
    if (Capacity == 0)
    {
      Capacity = InitialBufferSize;
    }

    Storage = reinterpret_cast<uint8_t *>(::malloc(Capacity));

    Size = Other.Size;
    std::memcpy(Storage, Other.data(), sizeof(value_type) * Size);
  }

  explicit ByteArray(unsigned InitialCapacity = InitialBufferSize)
      : Capacity(InitialCapacity), Size(0)
  {
    if (InitialCapacity == 0)
      Capacity = InitialBufferSize;

    Storage = reinterpret_cast<uint8_t *>(::malloc(Capacity));
  }

  ~ByteArray()
  {
    if (Storage != nullptr)
    {
      ::free(Storage);
      Storage = nullptr;
    }
  }

  /// Returns a pointer to the underlying array serving as element storage.
  value_type *data() { return Storage; }
  const value_type *data() const { return Storage; }

  /// Iterator utils.
  iterator begin() { return iterator(Storage); }
  const_iterator begin() const { return const_iterator(Storage); }
  iterator end() { return iterator(Storage + Size); }
  const_iterator end() const { return const_iterator(Storage + Size); }

  /// Checks if the array has no elements.
  bool empty() const { return (Size == 0); }

  /// Returns the number of elements in the array.
  unsigned size() const { return Size; }

  /// Returns the number of elements that the array has currently
  /// allocated space for.
  unsigned capacity() const { return Capacity; }

  /// Increase the capacity of the internal storage to the
  /// specified new value. Calling this function may invalidate
  /// all iterators. :NOTE: Calling this function when using
  /// external storage does nothing.
  void reserve(unsigned NewCapacity)
  {
    if (!NewCapacity || NewCapacity >= MaximumBufferSize || NewCapacity <= Capacity)
      return;

    Capacity = NewCapacity;
    Storage = reinterpret_cast<uint8_t *>(::realloc(Storage, Capacity));
  }

  /// Increase the capacity of the internal storage by Size
  /// bytes. Calling this function may invalidate all iterators.
  /// :NOTE: Calling this function when using external storage
  /// does nothing.
  void prealloc(unsigned _Size) { reserve(_Size + Size); }

  /// Resizes the array to contain Count elements.
  void resize(unsigned Count)
  {
    if (Count == 0 || Count == Size)
      return;

    // Current size is greater than Count: reduce the array size
    // to the first Count elements.
    if (Size > Count)
    {
      Size = Count;
      return;
    }

    // Current size is less than Count, additional elements are
    // appended and initialized with 0s.
    unsigned NewElemCount = Count - Size;
    prealloc(NewElemCount);

    if (Size + NewElemCount * sizeof(value_type) > Capacity)
      return;

    std::memset(Storage + Size, 0, NewElemCount * sizeof(value_type));
    Size = Count;
  }

  /// Appends Len Bytes from Value to the end of the container.
  /// When the new size is greater than capacity and internal
  /// storage is used then all iterators and references are
  /// invalidated otherwise causes undefined behaviour. NOTE:
  /// Calling this function with no data to push or with no
  /// length will do nothing.
  void push_back(const value_type *Value, unsigned Len)
  {
    if (!Value || !Len)
      return;

    prealloc(sizeof(value_type) * Len);

    if (Size + sizeof(value_type) * Len > Capacity)
      return;

    const volatile value_type *A = Value;

    for (unsigned i = 0; i != Len * sizeof(value_type); ++i)
      Storage[Size + i] = A[i];

    Size += sizeof(value_type) * Len;
  }

  /// Reads Len elements from the specified position and stores
  /// them into the input buffer. Trying to read more elements
  /// than available is undefined.
  void read(iterator Pos, value_type *Value, unsigned Len) const
  {
    Value[0] = 0;

    if (Len == 0 || Pos < begin() || Pos >= end() || (Pos + Len - 1) >= end())
      return;

    std::memcpy(Value, Pos, sizeof(value_type) * Len);
  }

private:
  value_type *Storage;
  unsigned Capacity;
  unsigned Size;
  
}; // namespace rmi

inline bool operator==(const ByteArray &LHS, const ByteArray &RHS)
{
  return std::equal(LHS.begin(), LHS.end(), RHS.begin(), RHS.end());
}

inline bool operator!=(const ByteArray &LHS, const ByteArray &RHS)
{
  return !(LHS == RHS);
}

} // namespace rmi

#endif // __INCLUDE_BYTEARRAY_H__