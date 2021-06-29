#include "module.hpp"
#include "complex.hpp"

using namespace Csdr;

template <typename T, typename U>
void Module<T, U>::setReader(RingbufferReader<T>* reader) {
    this->reader = reader;
}

template <typename T, typename U>
void Module<T, U>::setWriter(Writer<U>* writer) {
    this->writer = writer;
}

template class Module<short, short>;
template class Module<float, float>;
template class Module<complex<float>, float>;