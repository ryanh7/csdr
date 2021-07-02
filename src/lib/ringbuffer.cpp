#include "ringbuffer.hpp"
#include "complex.hpp"

#include <sys/mman.h>

using namespace Csdr;

template <typename T>
Ringbuffer<T>::Ringbuffer(size_t size) {
    data = allocate_mirrored(size);
    if (data == nullptr) {
        throw BufferError("unable to allocate ringbuffer memory");
    }
}

template <typename T>
T* Ringbuffer<T>::allocate_mirrored(size_t size) {

#ifdef PAGESIZE
	static constexpr unsigned int PAGE_SIZE = PAGESIZE;
#else
	static const unsigned int PAGE_SIZE = ::sysconf(_SC_PAGESIZE);
#endif

    size_t bytes = ((sizeof(T) * size + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
    if (bytes % sizeof(T)) {
        throw BufferError("unable to align buffer with page size");
    }
    this->size = bytes / sizeof(T);

    int counter = 10;
    while (counter-- > 0) {
        unsigned char* addr = static_cast<unsigned char*>(::mmap(NULL, 2 * bytes, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));

        if (addr == MAP_FAILED) {
            continue;
        }

        addr = static_cast<unsigned char*>(::mremap(addr, 2 * bytes, bytes, 0));
        if (addr == MAP_FAILED) {
            continue;
        }

        unsigned char* addr2 = static_cast<unsigned char*>(::mremap(addr, 0, bytes, MREMAP_FIXED | MREMAP_MAYMOVE, addr + bytes));
        if (addr2 == MAP_FAILED) {
            ::munmap(addr, bytes);
            continue;
        }

        if (addr2 != addr + bytes) {
            ::munmap(addr, bytes);
            ::munmap(addr2, bytes);
            continue;
        }

        return (T*) addr;
    }

    return nullptr;
}

template <typename T>
Ringbuffer<T>::~Ringbuffer() {
    if (data != nullptr) {
        unsigned char* addr = (unsigned char*) data;
        size_t bytes = this->size * sizeof(T);
        ::munmap(addr, bytes);
        ::munmap(addr + bytes, bytes);
    }
}

template <typename T>
size_t Ringbuffer<T>::writeable() {
    return size - 1;
}

template<typename T>
T* Ringbuffer<T>::getPointer(size_t pos) {
    return data + pos;
}

template <typename T>
T* Ringbuffer<T>::getWritePointer() {
    return getPointer(write_pos);
}

template <typename T>
void Ringbuffer<T>::advance(size_t& what, size_t how_much) {
    what = (what + how_much) % size;
}

template <typename T>
void Ringbuffer<T>::advance(size_t how_much) {
    advance(write_pos, how_much);
}

template <typename T>
size_t Ringbuffer<T>::available(size_t read_pos) {
    return (size + write_pos - read_pos) % size;
}

template<typename T>
size_t Ringbuffer<T>::getWritePos() {
    return write_pos;
}

template <typename T>
RingbufferReader<T>::RingbufferReader(Ringbuffer<T>* buffer) {
    this->buffer = buffer;
    read_pos = buffer->getWritePos();
}

template <typename T>
size_t RingbufferReader<T>::available() {
    return buffer->available(read_pos);
}

template <typename T>
T* RingbufferReader<T>::getReadPointer() {
    return buffer->getPointer(read_pos);
}

template <typename T>
void RingbufferReader<T>::advance(size_t how_much) {
    buffer->advance(read_pos, how_much);
}

// compile templates for all the possible variations
template class Ringbuffer<char>;
template class RingbufferReader<char>;

template class Ringbuffer<unsigned char>;
template class RingbufferReader<unsigned char>;

template class Ringbuffer<short>;
template class RingbufferReader<short>;

template class Ringbuffer<float>;
template class RingbufferReader<float>;

template class Ringbuffer<complex<float>>;
template class RingbufferReader<complex<float>>;