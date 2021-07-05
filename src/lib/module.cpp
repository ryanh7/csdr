#include "module.hpp"

#include <algorithm>

using namespace Csdr;

template <typename T, typename U>
void Module<T, U>::setReader(Reader<T>* reader) {
    this->reader = reader;
}

template <typename T, typename U>
void Module<T, U>::setWriter(Writer<U>* writer) {
    this->writer = writer;
}

template <typename T, typename U>
bool AnyLengthModule<T, U>::canProcess() {
    return getWorkSize() > 0;
}

template <typename T, typename U>
size_t AnyLengthModule<T, U>::getWorkSize() {
    return std::min({this->reader->available(), this->writer->writeable(), maxLength()});
}

template <typename T, typename U>
void AnyLengthModule<T, U>::process() {
    size_t available = getWorkSize();
    process(this->reader->getReadPointer(), this->writer->getWritePointer(), available);
    this->reader->advance(available);
    this->writer->advance(available);
}

template <typename T, typename U>
bool FixedLengthModule<T, U>::canProcess() {
    size_t available = std::min(this->reader->available(), this->writer->writeable());
    return available > getLength();
}

template <typename T, typename U>
void FixedLengthModule<T, U>::process () {
    size_t length = getLength();
    process(this->reader->getReadPointer(), this->writer->getWritePointer());
    this->reader->advance(length);
    this->writer->advance(length);
}

namespace Csdr {
    template class Module<short, short>;
    template class Module<float, float>;
    template class Module<complex<float>, float>;
    template class Module<short, float>;
    template class Module<float, short>;
    template class Module<complex<float>, complex<float>>;
    template class Module<short, unsigned char>;
    template class Module<unsigned char, short>;

    template class AnyLengthModule<short, short>;
    template class AnyLengthModule<float, float>;
    template class AnyLengthModule<complex<float>, float>;
    template class AnyLengthModule<short, float>;
    template class AnyLengthModule<float, short>;
    template class AnyLengthModule<complex<float>, complex<float>>;

    template class FixedLengthModule<float, float>;
    template class FixedLengthModule<complex<float>, complex<float>>;
}