#include "writer.hpp"

#include <iostream>

using namespace Csdr;

template <typename T>
size_t StdoutWriter<T>::write(T* data, size_t size) {
    std::cout.write((const char*) data, sizeof(T) * size);
    std::cout.flush();
    return size;
}

template class StdoutWriter<char>;
template class StdoutWriter<unsigned char>;
template class StdoutWriter<short>;
template class StdoutWriter<float>;