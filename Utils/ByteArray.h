#ifndef __INCLUDE_BYTEARRAY_H__
#define __INCLUDE_BYTEARRAY_H__

#include <fstream>

namespace rmi {

  // A class that implements a dynamic Byte Array.
  // Disclaimer: just for testing purposes (not intended to be a complete implementation).
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
        storage = reinterpret_cast<uint8_t*>(::malloc(capacity));
        std::memcpy(storage, other.data(), sizeof(value_type) * size);
      }

      explicit ByteArray(unsigned initialCapacity = initialBufferSize) : capacity(initialCapacity), size(0) {
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

      void push_back(const value_type* value, unsigned len) {
        unsigned sizeOfNewContent= sizeof(value_type) * len;
        capacity = size + sizeOfNewContent;
        storage = reinterpret_cast<uint8_t*>(::realloc(storage, capacity));

        for (unsigned i = 0; i != sizeOfNewContent; ++i)
          storage[size + i] = value[i];

        size += sizeOfNewContent;
      }

      void read(iterator pos, value_type* value, unsigned len) const {
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