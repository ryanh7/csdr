#include "writer.hpp"
#include "complex.hpp"

#include <unistd.h>

using namespace Csdr;

template <typename T>
StdoutWriter<T>::StdoutWriter(size_t buffer_size):
    buffer_size(buffer_size),
    buffer((T*) malloc(sizeof(T) * buffer_size))
{}

template <typename T>
StdoutWriter<T>::StdoutWriter(): StdoutWriter(10240) {}

template <typename T>
StdoutWriter<T>::~StdoutWriter() {
    free(buffer);
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
    write(fileno(stdout), (const char*) buffer, sizeof(T) * how_much);
}

template<typename T>
VoidWriter<T>::VoidWriter(size_t buffer_size):
    buffer_size(buffer_size),
    data((T*) malloc(sizeof(T) * buffer_size))
{}

template <typename T>
VoidWriter<T>::VoidWriter(): VoidWriter(1024) {}

template <typename T>
VoidWriter<T>::~VoidWriter<T>() {
    free(data);
}

template <typename T>
size_t VoidWriter<T>::writeable() {
    return buffer_size;
}

template <typename T>
T* VoidWriter<T>::getWritePointer() {
    return data;
}

namespace Csdr {
    template class StdoutWriter<char>;
    template class StdoutWriter<unsigned char>;
    template class StdoutWriter<short>;
    template class StdoutWriter<float>;
    template class StdoutWriter<complex<float>>;

    template class VoidWriter<complex<float>>;
}