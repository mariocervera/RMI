#ifndef __INCLUDE_BYTEARRAY_H__
#define __INCLUDE_BYTEARRAY_H__

#include <fstream>

namespace elint
{

/// This class provides an array of bytes. Use it when you need
/// to store an array of raw bytes coming from a source. It
/// basically has two modes of operation: using internal storage,
/// the array is dynamic and grows as new elements are inserted,
/// similar to std::vector. The second mode uses external storage
/// that has to be provided by the user having its size fixed and
/// not being growable, so it must be assured that enough space
/// is allocated.
class ByteArray
{
  enum
  {
    InitialBufferSize = 512u,
    MaximumBufferSize = 4u * 1024u * 1024u
  };

public:
  /// Common typedefs.
  using value_type = uint8_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using iterator = value_type *;
  using const_iterator = const value_type *;

  ByteArray(const ByteArray &Other)
      : Capacity(Other.Capacity), Size(Other.Size), SelfStorage(true),
        InitializationError(false), OperationError(false), Error(0)
  {
    if (Capacity == 0)
    {
      Capacity = InitialBufferSize;
      InitializationError = true;
    }

    Storage = reinterpret_cast<uint8_t *>(::malloc(Capacity));

    Size = Other.Size;
    std::memcpy(Storage, Other.data(), sizeof(value_type) * Size);
  }

  /// Constructs an empty byte array with internal storage and
  /// dynamic capacity.
  explicit ByteArray(unsigned InitialCapacity = InitialBufferSize)
      : Capacity(InitialCapacity), Size(0), SelfStorage(true),
        InitializationError(false), OperationError(false), Error(0)
  {
    if (InitialCapacity == 0)
    {
      Capacity = InitialBufferSize;
      InitializationError = true;
    }

    Storage = reinterpret_cast<uint8_t *>(::malloc(Capacity));
  }

  /// Destroys the byte array and its internal storage if
  /// applicable.
  ~ByteArray()
  {
    if (SelfStorage && Storage != nullptr)
    {
      ::free(Storage);
      Storage = nullptr;
    }
  }

  /// Returns a pointer to the underlying array serving as
  /// element storage.
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
    if (InitializationError)
    {
      OperationError = true;
      return;
    }

    // Do nothing if NewCapacity is zero.
    if (!NewCapacity)
      return;

    // Do nothing if we do not own the storage.
    if (!SelfStorage)
      return;

    if (NewCapacity >= MaximumBufferSize)
    {
      OperationError = true;
      return;
    }

    // Do nothing when we already have enough capacity.
    if (NewCapacity <= Capacity)
      return;

    // Resize the storage.
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
    if (Count == 0)
    {
      OperationError = true;
      return;
    }

    // Easy case: we already match the required size.
    if (Count == Size)
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
    {
      OperationError = true;
      return;
    }

    std::memset(Storage + Size, 0, NewElemCount * sizeof(value_type));
    Size = Count;
  }

  /// Removes all elements from the array. Calling this function
  /// invalidates any references and interators.
  void clear() { Size = 0; }

  /// Appends Len Bytes from Value to the end of the container.
  /// When the new size is greater than capacity and internal
  /// storage is used then all iterators and references are
  /// invalidated otherwise causes undefined behaviour. NOTE:
  /// Calling this function with no data to push or with no
  /// length will do nothing.
  void push_back(const value_type *Value, unsigned Len)
  {
    if (!Value)
      return;

    if (!Len)
      return;

    prealloc(sizeof(value_type) * Len);

    if (Size + sizeof(value_type) * Len > Capacity)
    {
      OperationError = true;
      return;
    }

    const volatile value_type *A = Value;

    // :TODO: habra que investigar esto!!
    for (unsigned i = 0; i != Len * sizeof(value_type); ++i)
      Storage[Size + i] = A[i];

    // std::memcpy(Storage + Size, Value, sizeof(value_type) *
    // Len);
    Size += sizeof(value_type) * Len;
  }

  /// Appends Len Bytes from Offset position of the given Data to
  /// the end of the container. When the new size is greater than
  /// capacity and internal storage is used then all iterators
  /// and references are invalidated otherwise causes undefined
  /// behaviour. NOTE: Calling this function with no data to push
  /// or with no length will do nothing.
  void push_back(const ByteArray &Data, unsigned Len, unsigned Offset = 0u)
  {
    if (Data.empty())
      return;

    if (!Len)
      return;

    prealloc(sizeof(value_type) * Len);

    if (Size + sizeof(value_type) * Len > Capacity)
    {
      OperationError = true;
      return;
    }

    if ((Offset + Len) > Data.size())
    {
      OperationError = true;
      return;
    }

    std::memcpy(Storage + Size, Data.begin() + Offset,
                sizeof(value_type) * Len);

    Size += sizeof(value_type) * Len;
  }

  /// Reads Len elements from the specified position and stores
  /// them into the input buffer. Trying to read more elements
  /// than available is undefined.
  void read(iterator Pos, value_type *Value, unsigned Len) const
  {
    // Initialize Value to avoid Warning.
    Value[0] = 0;
    if (Len == 0)
      return;

    if (Pos < begin())
    {
      OperationError = true;
      return;
    }

    if (Pos >= end())
    {
      OperationError = true;
      return;
    }

    if ((Pos + Len - 1) >= end())
    {
      return;
    }

    std::memcpy(Value, Pos, sizeof(value_type) * Len);
  }

private:
  value_type *Storage;
  unsigned Capacity;
  unsigned Size;
  const bool SelfStorage;
  mutable bool InitializationError;
  mutable bool OperationError;
  value_type Error;
}; // namespace elint

inline bool operator==(const ByteArray &LHS, const ByteArray &RHS)
{
  return std::equal(LHS.begin(), LHS.end(), RHS.begin(), RHS.end());
}

inline bool operator!=(const ByteArray &LHS, const ByteArray &RHS)
{
  return !(LHS == RHS);
}

} // namespace elint

#endif // __INCLUDE_BYTEARRAY_H__
