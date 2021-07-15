#include "sink.hpp"

using namespace Csdr;

template <typename T>
void Sink<T>::setReader(Reader<T>* reader) {
    this->reader = reader;
}

template <typename T>
Reader<T>* Sink<T>::getReader() {
    return reader;
}

template<typename T>
bool Sink<T>::hasReader() {
    return reader != nullptr;
}

namespace Csdr {
    template class Sink<short>;
    template class Sink<float>;
    template class Sink<complex<float>>;
    template class Sink<unsigned char>;
}