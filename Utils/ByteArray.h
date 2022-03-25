#ifndef __INCLUDE_BYTEARRAY_H__
#define __INCLUDE_BYTEARRAY_H__

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iomanip>
#include <stdio.h>

namespace elint
{

static auto logError = [](const std::string &ErrorString) {
  std::ofstream outfile;

  outfile.open("ByteArray.log", std::ios_base::app);
  outfile << "[";
  auto now = std::chrono::system_clock::now();
  auto itt = std::chrono::system_clock::to_time_t(now);
  outfile << std::put_time(gmtime(&itt), "%FT%TZ");
  outfile << "][ByteArray]: " + ErrorString;
  outfile << std::endl;
  outfile.close();
};

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
      logError(std::string("Invalid capacity. Using a capacity of ") +
               std::to_string(InitialBufferSize) + " instead.");
      InitializationError = true;
    }

    Storage = reinterpret_cast<uint8_t *>(::malloc(Capacity));

    Size = Other.Size;
    std::memcpy(Storage, Other.data(), sizeof(value_type) * Size);
  }

  ByteArray &operator=(const ByteArray &Other)
  {
    if (InitializationError)
    {
      logError("Working with InitializationError");
      OperationError = true;
      return const_cast<ByteArray &>(Other);
    }

    if (SelfStorage == 0)
    {
      logError("Cannot assign a non selfstorage bytearray. Returning "
               "input ByteArray");
      OperationError = true;
      return const_cast<ByteArray &>(Other);
    }

    // Check the array capacity.
    if (Other.Capacity > Capacity)
      reserve(Other.Capacity);
    if (OperationError)
    {
      logError("Error on resize");
      OperationError = true;
      return const_cast<ByteArray &>(Other);
    }

    Size = Other.Size;
    // Perform the copy.
    std::memcpy(Storage, Other.data(), sizeof(value_type) * Size);

    return *this;
  }

  /// Constructs a byte array with the specfied storage location
  /// and fixed capacity. The caller must ensure the storage will
  /// not be deleted as long the byte array exists.
  ByteArray(value_type *Storage, unsigned Capacity)
      : Storage(Storage), Capacity(Capacity), Size(0), SelfStorage(false),
        InitializationError(false), OperationError(false), Error(0)
  {
    if (Storage == nullptr)
    {
      logError("Invalid Storage pointer");
      InitializationError = true;
      const_cast<bool &>(SelfStorage) = true;
    }

    if (Capacity == 0)
    {
      logError("Invalid capacity");
      Capacity = InitialBufferSize;
      InitializationError = true;
    }
  }

  /// Constructs an empty byte array with internal storage and
  /// dynamic capacity.
  explicit ByteArray(unsigned InitialCapacity = InitialBufferSize)
      : Capacity(InitialCapacity), Size(0), SelfStorage(true),
        InitializationError(false), OperationError(false), Error(0)
  {
    if (InitialCapacity == 0)
    {
      logError("Invalid capacity");
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

  /// Returns a reference to the element at the specified
  /// position.
  reference at(unsigned Pos)
  {
    if (InitializationError)
    {
      logError("Working with InitializationError");
      OperationError = true;
      return Storage[0];
    }
    if (Pos >= Size)
    {
      logError("Invalid position");
      OperationError = true;
      Pos = Size - 1;
    }
    return Storage[Pos];
  }
  const_reference at(unsigned Pos) const
  {
    if (InitializationError)
    {
      logError("Working with InitializationError");
      OperationError = true;
      return Storage[0];
    }
    if (Pos >= Size)
    {
      logError("Invalid position");
      OperationError = true;
      Pos = Size - 1;
    }
    return Storage[Pos];
  }

  reference operator[](unsigned Pos)
  {
    if (InitializationError)
    {
      logError("Working with InitializationError");
      OperationError = true;
      return Storage[0];
    }
    if (Pos >= Size)
    {
      logError("Invalid position");
      OperationError = true;
      Pos = Size - 1;
    }
    return Storage[Pos];
  }
  const_reference operator[](unsigned Pos) const
  {
    if (InitializationError)
    {
      logError("Working with InitializationError");
      OperationError = true;
      return Storage[0];
    }
    if (Pos >= Size)
    {
      logError("Invalid position");
      OperationError = true;
      Pos = Size - 1;
    }
    return Storage[Pos];
  }

  /// Returns a reference to the first element in the array.
  /// :NOTE: Calling front on an empty array is undefined.
  reference front()
  {
    if (InitializationError)
    {
      logError("Working with InitializationError");
      OperationError = true;
      return static_cast<value_type &>(Error);
    }
    if (empty())
    {
      logError("Array is empty");
      OperationError = true;
      return static_cast<value_type &>(Error);
    }
    return Storage[0];
  }
  const_reference front() const
  {
    if (InitializationError)
    {
      logError("Working with InitializationError");
      OperationError = true;
      return const_cast<value_type &>(Error);
    }
    if (empty())
    {
      logError("Array is empty");
      OperationError = true;
      return const_cast<value_type &>(Error);
    }
    return Storage[0];
  }

  /// Returns a reference to the last element in the array.
  /// :NOTE: Calling back on an empty array is undefined.
  reference back()
  {
    if (InitializationError)
    {
      logError("Working with InitializationError");
      OperationError = true;
      return static_cast<value_type &>(Error);
    }
    if (empty())
    {
      logError("Array is empty");
      OperationError = true;
      return static_cast<value_type &>(Error);
    }
    return Storage[Size ? Size - 1 : 0];
  }
  const_reference back() const
  {
    if (InitializationError)
    {
      logError("Working with InitializationError");
      OperationError = true;
      return const_cast<value_type &>(Error);
    }
    if (empty())
    {
      logError("Array is empty");
      OperationError = true;
      return const_cast<value_type &>(Error);
    }
    return Storage[Size ? Size - 1 : 0];
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

  /// Returns the number of bytes that are available for reading.
  /// Same as size().
  unsigned bytesAvailable() const { return size(); }

  /// Increase the capacity of the internal storage to the
  /// specified new value. Calling this function may invalidate
  /// all iterators. :NOTE: Calling this function when using
  /// external storage does nothing.
  void reserve(unsigned NewCapacity)
  {
    if (InitializationError)
    {
      logError("Working with InitializationError");
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
      logError("New capacity is too large");
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
      logError("Invalid count");
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
      logError("Cannot resize:\tCount:" + std::to_string(Count));
      logError("Cannot resize:\tNewElemCount:" + std::to_string(NewElemCount));
      OperationError = true;
      return;
    }

    std::memset(Storage + Size, 0, NewElemCount * sizeof(value_type));
    Size = Count;
  }

  /// Removes all elements from the array. Calling this function
  /// invalidates any references and interators.
  void clear() { Size = 0; }

  /// Appends the given value to the end of the container. When
  /// the new size is greater than capacity and internal storage
  /// is used then all iterators and references are invalidated
  /// otherwise causes undefined behaviour.
  void push_back(const value_type &Value)
  {
    prealloc(sizeof(value_type));

    if (Size + sizeof(value_type) > Capacity)
    {
      logError("Cannot insert a new element");
      OperationError = true;
      return;
    }
    std::memcpy(Storage + Size, &Value, sizeof(value_type));
    Size += sizeof(value_type);
  }

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
      logError("Cannot insert a new element");
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
      logError("Cannot insert a new element");
      OperationError = true;
      return;
    }

    if ((Offset + Len) > Data.size())
    {
      logError("Data out of range");
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
      logError("Pos is out of bounds");
      OperationError = true;
      return;
    }

    if (Pos >= end())
    {
      logError("Pos is past the end of the vector");
      OperationError = true;
      return;
    }

    if ((Pos + Len - 1) >= end())
    {
      logError("Not enough bytes available to read");
      return;
    }

    std::memcpy(Value, Pos, sizeof(value_type) * Len);
  }

  /// Function to return if there has been an operation error.
  /// Returns false if there has been an error, otherwise, true.
  /// NOTE: This function should be called after calling each one
  /// of the ByteArray methods to assure that the operation
  /// performed has succeeded.
  const bool isOperationValid() const
  {
    if (InitializationError)
      return false;

    bool Status = !OperationError;
    OperationError = false;
    return Status;
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

/// Static version of the byte array, useful when you know the
/// required size beforehand to avoid using dynamic memory.
template <unsigned N> class StaticByteArray : public ByteArray
{
  uint8_t Buffer[N];

public:
  StaticByteArray() : ByteArray(Buffer, N) {}

  StaticByteArray(const StaticByteArray &) = delete;
  StaticByteArray &operator=(const StaticByteArray &) = delete;
};

} // namespace elint

#endif // __INCLUDE_BYTEARRAY_H__
