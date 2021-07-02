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
void AnyLengthModule<T, U>::process() {
    size_t available;
    while (available = this->reader->available()) {
        size_t work_size = std::min({available, this->writer->writeable(), maxLength()});
        process(this->reader->getReadPointer(), this->writer->getWritePointer(), work_size);
        this->reader->advance(work_size);
        this->writer->advance(work_size);
    }
}

template <typename T, typename U>
void FixedLengthModule<T, U>::process () {
    size_t available;
    size_t length;
    while ((available = this->reader->available()) > (length = getLength())) {
        process(this->reader->getReadPointer(), this->writer->getWritePointer());
        this->reader->advance(length);
        this->writer->advance(length);
    }
}

namespace Csdr {
    template class Module<short, short>;
    template class Module<float, float>;
    template class Module<complex<float>, float>;
    template class Module<short, float>;
    template class Module<float, short>;
    template class Module<complex<float>, complex<float>>;

    template class AnyLengthModule<short, short>;
    template class AnyLengthModule<float, float>;
    template class AnyLengthModule<complex<float>, float>;
    template class AnyLengthModule<short, float>;
    template class AnyLengthModule<float, short>;
    template class AnyLengthModule<complex<float>, complex<float>>;

    template class FixedLengthModule<float, float>;
    template class FixedLengthModule<complex<float>, complex<float>>;
}