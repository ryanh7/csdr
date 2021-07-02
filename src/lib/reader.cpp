#include "reader.hpp"

using namespace Csdr;

template <typename T>
MemoryReader<T>::MemoryReader(T *data, size_t size): data(data), size(size) {}

template <typename T>
size_t MemoryReader<T>::available() {
    return size - read_pos;
}

template <typename T>
T* MemoryReader<T>::getReadPointer() {
    return data + read_pos;
}

template <typename T>
void MemoryReader<T>::advance(size_t how_much) {
    read_pos += how_much;
}

template <typename T>
void MemoryReader<T>::rewind() {
    read_pos = 0;
}