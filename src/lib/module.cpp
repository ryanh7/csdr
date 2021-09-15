#include "module.hpp"

#include <algorithm>

using namespace Csdr;

template <typename T, typename U>
Module<T, U>::~Module() {
    std::lock_guard<std::mutex> lock(processMutex);
}

template <typename T, typename U>
void Module<T, U>::wait() {
    waitingReader = this->getReader();
    waitingReader->wait();
    waitingReader = nullptr;
}

template <typename T, typename U>
void Module<T, U>::unblock() {
    auto r = waitingReader;
    if (r != nullptr) {
        r->unblock();
    }
}

template <typename T, typename U>
void Module<T, U>::setWriter(Writer<U> *writer) {
    std::lock_guard<std::mutex> lock(processMutex);
    Source<U>::setWriter(writer);
}

template <typename T, typename U>
void Module<T, U>::setReader(Reader<T> *reader) {
    std::lock_guard<std::mutex> lock(processMutex);
    Sink<T>::setReader(reader);
    if (reader != waitingReader) waitingReader = nullptr;
}

template <typename T, typename U>
bool AnyLengthModule<T, U>::canProcess() {
    std::lock_guard<std::mutex> lock(this->processMutex);
    return getWorkSize() > 0;
}

template <typename T, typename U>
size_t AnyLengthModule<T, U>::getWorkSize() {
    return std::min({this->reader->available(), this->writer->writeable(), maxLength()});
}

template <typename T, typename U>
void AnyLengthModule<T, U>::process() {
    std::lock_guard<std::mutex> lock(this->processMutex);
    size_t available = getWorkSize();
    process(this->reader->getReadPointer(), this->writer->getWritePointer(), available);
    this->reader->advance(available);
    this->writer->advance(available);
}

template <typename T, typename U>
bool FixedLengthModule<T, U>::canProcess() {
    std::lock_guard<std::mutex> lock(this->processMutex);
    size_t length = getLength();
    return (this->reader->available() > length && this->writer->writeable() > length);
}

template <typename T, typename U>
void FixedLengthModule<T, U>::process () {
    std::lock_guard<std::mutex> lock(this->processMutex);
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
    template class Module<float, unsigned char>;
    template class Module<complex<float>, unsigned char>;
    template class Module<unsigned char, unsigned char>;
    template class Module<complex<float>, complex<short>>;

    template class AnyLengthModule<short, short>;
    template class AnyLengthModule<float, float>;
    template class AnyLengthModule<complex<float>, float>;
    template class AnyLengthModule<short, float>;
    template class AnyLengthModule<float, short>;
    template class AnyLengthModule<complex<float>, complex<float>>;
    template class AnyLengthModule<complex<float>, unsigned char>;
    template class AnyLengthModule<complex<float>, complex<short>>;

    template class FixedLengthModule<float, float>;
    template class FixedLengthModule<complex<float>, complex<float>>;
}