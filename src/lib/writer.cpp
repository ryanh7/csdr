#include "writer.hpp"

#include <iostream>

using namespace Csdr;

template <typename T>
StdoutWriter<T>::StdoutWriter(size_t buffer_size): buffer_size(buffer_size) {
    buffer = (T*) malloc(sizeof(T) * buffer_size);
}

template <typename T>
StdoutWriter<T>::StdoutWriter(): StdoutWriter(1024) {}

template <typename T>
StdoutWriter<T>::~StdoutWriter() {
    free(buffer);
}

template <typename T>
size_t StdoutWriter<T>::write(T* data, size_t size) {
    std::cout.write((const char*) data, sizeof(T) * size);
    std::cout.flush();
    return size;
}

template <typename T>
size_t StdoutWriter<T>::writeable() {
    return buffer_size;
}

template <typename T>
T* StdoutWriter<T>::getWritePointer() {
    return buffer;
}

template <typename T>
void StdoutWriter<T>::advance(size_t how_much) {
    write(buffer, how_much);
}

template class StdoutWriter<char>;
template class StdoutWriter<unsigned char>;
template class StdoutWriter<short>;
template class StdoutWriter<float>;