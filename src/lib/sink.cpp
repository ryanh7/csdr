#include "sink.hpp"

using namespace Csdr;

template <typename T>
void Sink<T>::setReader(Reader<T>* reader) {
    if (reader == this->reader) return;
    auto oldReader = this->reader;
    this->reader = reader;
    // if we had a reader before, there's a chance we're still wait()ing on it in a thread.
    // this makes sure we start reading on the new reader immediately.
    if (oldReader != nullptr) oldReader->unblock();
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