#include "ringbuffer.hpp"

using namespace Csdr;

template <typename T>
Ringbuffer<T>::Ringbuffer(size_t size) {
    this->size = size;
    data = (T*) malloc(sizeof(T) * size);
}

template <typename T>
Ringbuffer<T>::~Ringbuffer() {
    free(data);
}

template <typename T>
size_t Ringbuffer<T>::writeable() {
    return size - write_pos;
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
    if (write_pos >= read_pos) {
        return write_pos - read_pos;
    } else {
        return size - read_pos;
    }
}

template <typename T>
RingbufferReader<T>::RingbufferReader(Ringbuffer<T>* buffer) {
    this->buffer = buffer;
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