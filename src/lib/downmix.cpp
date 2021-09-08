#include "downmix.hpp"

using namespace Csdr;

template <typename T>
Downmix<T>::Downmix(unsigned int channels): channels(channels) {}

template <typename T>
bool Downmix<T>::canProcess() {
    return this->reader->available() >= channels && this->writer->writeable() > 0;
}

template <typename T>
void Downmix<T>::process() {
    std::lock_guard<std::mutex> lock(this->processMutex);
    size_t numSamples = std::min(this->reader->available() / channels, this->writer->writeable());
    T* input = this->reader->getReadPointer();
    T* output = this->writer->getWritePointer();
    for (size_t i = 0; i < numSamples; i++) {
        T sum = 0;
        for (unsigned int k = 0; k < channels; k++) {
            sum += input[i * channels + k] / channels;
        }
        output[i] = sum;
    }
    this->reader->advance(numSamples * channels);
    this->writer->advance(numSamples);
}

namespace Csdr {
    template class Downmix<short>;
}