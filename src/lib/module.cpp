#include "module.hpp"
#include "complex.hpp"

#include <algorithm>

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

template class AnyLengthModule<short, short>;
template class AnyLengthModule<float, float>;
template class AnyLengthModule<complex<float>, float>;
