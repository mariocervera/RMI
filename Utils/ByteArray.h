#ifndef __INCLUDE_BYTEARRAY_H__
#define __INCLUDE_BYTEARRAY_H__

#include <fstream>

namespace rmi {

  class ByteArray {

    enum {
      initialBufferSize = 512u,
      maximumBufferSize = 4u * 1024u * 1024u
    };

  public:
    using value_type = uint8_t;
    using iterator = value_type*;
    using const_iterator = const value_type*;

    ByteArray(const ByteArray& other) : capacity(other.capacity), size(other.size) {

      if (capacity == 0)
        capacity = initialBufferSize;

      storage = reinterpret_cast<uint8_t*>(::malloc(capacity));

      size = other.size;
      std::memcpy(storage, other.data(), sizeof(value_type) * size);
    }

    explicit ByteArray(unsigned initialCapacity = initialBufferSize) : capacity(initialCapacity), size(0) {

      if (initialCapacity == 0)
        capacity = initialBufferSize;

      storage = reinterpret_cast<uint8_t*>(::malloc(capacity));
    }

    ~ByteArray() {
      if (storage != nullptr) {
        ::free(storage);
        storage = nullptr;
      }
    }

    value_type* data() { return storage; }
    const value_type* data() const { return storage; }

    iterator begin() { return iterator(storage); }
    const_iterator begin() const { return const_iterator(storage); }
    iterator end() { return iterator(storage + size); }
    const_iterator end() const { return const_iterator(storage + size); }

    bool empty() const { return size == 0; }

    void reserve(unsigned newCapacity) {
      if (!newCapacity || newCapacity >= maximumBufferSize || newCapacity <= capacity)
        return;

      capacity = newCapacity;
      storage = reinterpret_cast<uint8_t*>(::realloc(storage, capacity));
    }

    void prealloc(unsigned _size) { reserve(size + _size); }

    void push_back(const value_type* value, unsigned len) {
      if (!value || !len)
        return;

      prealloc(sizeof(value_type) * len);

      if (size + sizeof(value_type) * len > capacity)
        return;

      const volatile value_type* a = value;

      for (unsigned i = 0; i != len * sizeof(value_type); ++i)
        storage[size + i] = a[i];

      size += sizeof(value_type) * len;
    }

    void read(iterator pos, value_type* value, unsigned len) const {
      value[0] = 0;

      if (len == 0 || pos < begin() || pos >= end() || (pos + len - 1) >= end())
        return;

      std::memcpy(value, pos, sizeof(value_type) * len);
    }

  private:
    value_type* storage;
    unsigned capacity;
    unsigned size;
  };

}

#endif // __INCLUDE_BYTEARRAY_H__