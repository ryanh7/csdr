#include "fir.hpp"
#include "complex.hpp"
#include "fmv.h"

#include <cmath>
#include <cstring>

using namespace Csdr;

template<typename T>
SparseView<T> Filter<T>::sparse(T* data) {
    return SparseView<T>(data, this);
}

template <typename T>
void Filter<T>::apply(T *input, T *output, size_t size) {
    for (size_t i = 0; i < size; i++) {
        output[i] = processSample(input, i);
    }
}

template<typename T>
SparseView<T>::SparseView(T *data, Filter<T> *filter):
    data(data),
    filter(filter)
{}

template<typename T>
T SparseView<T>::operator[](size_t index) {
    return filter->processSample(data, index);
}

template<typename T, typename U>
FirFilter<T, U>::FirFilter(unsigned int length) {
    allocateTaps(length);
}

template <typename T, typename U>
FirFilter<T, U>::FirFilter(U* taps, unsigned int length): FirFilter(length) {
    // better to copy the taps to our memory since that is aligned
    std::memcpy(this->taps, taps, sizeof(U) * length);
}

template<typename T, typename U>
FirFilter<T, U>::~FirFilter() {
    free(taps);
}

template<typename T, typename U>
T FirFilter<T, U>::processSample(T *data, size_t index) {
    return processSample_fmv(data, index);
}

template<>
CSDR_TARGET_CLONES
complex<float> FirFilter<complex<float>, float>::processSample_fmv(complex<float> *data, size_t index) {
    complex<float> acc = {0, 0};
    for (int ti = 0; ti < taps_length; ti++) {
        acc += data[index + ti] * taps[ti];
    }
    return acc;
}

template <typename T, typename U>
T FirFilter<T, U>::processSample_fmv(T *data, size_t index) {
    T acc = 0;
    for (int ti = 0; ti < taps_length; ti++) {
        acc += data[index + ti] * taps[ti];
    }
    return acc;
}

template<typename T, typename U>
unsigned int FirFilter<T, U>::filterLength(float transition) {
    unsigned int result = 4.0 / transition;
    if (result % 2 == 0) result++; //number of symmetric FIR filter taps should be odd
    return result;
}

template<typename T, typename U>
unsigned int FirFilter<T, U>::getLength() {
    return taps_length;
}

template<typename T, typename U>
void FirFilter<T, U>::allocateTaps(unsigned int length) {
#ifdef NEON_OPTS
#define NEON_ALIGNMENT (4 * 4 * 2)
    unsigned int padded_taps_length = length;
    padded_taps_length = length + (NEON_ALIGNMENT / 4) -1 - ((length + (NEON_ALIGNMENT / 4) - 1) % (NEON_ALIGNMENT / 4));

    taps = (float*) malloc((padded_taps_length + NEON_ALIGNMENT) * sizeof(U));
    taps = (float*) ((((size_t) taps) + NEON_ALIGNMENT - 1) & ~(NEON_ALIGNMENT - 1));
    for (int i = 0; i < padded_taps_length - length; i++) taps[taps_length + i] = 0;
    taps_length = padded_taps_length;
#else
    taps = (U*) malloc(length * sizeof(U));
    taps_length = length;
#endif
}

template<typename T, typename U>
void FirFilter<T, U>::normalize() {
    //Normalize filter kernel
    U sum = 0.0;
    for (int i = 0; i < taps_length; i++) sum += taps[i];
    for (int i = 0; i < taps_length; i++) taps[i] = taps[i] / sum;
}

template<typename T>
LowPassFilter<T>::LowPassFilter(float cutoff, float transition, Window* window): FirFilter<T, float>(FirFilter<T, float>::filterLength(transition)) {
    //Generates symmetric windowed sinc FIR filter real taps
    //  cutoff_rate is (cutoff frequency/sampling frequency)
    //Explanation at Chapter 16 of dspguide.com
    int middle = this->taps_length / 2;
    this->taps[middle] = 2 * M_PI * cutoff * window->kernel(0);
    for (int i = 1; i <= middle; i++)  {
        this->taps[middle - i] = this->taps[middle + i] = (sin(2 * M_PI * cutoff * i) / i) * window->kernel((float) i / middle);
    }
    this->normalize();
}

template <typename T, typename U>
FirModule<T, U>::FirModule(FirFilter<T, U> *filter): filter(filter) {}

template <typename T, typename U>
bool FirModule<T, U>::canProcess() {
    return this->reader->available() > filter->getLength() && this->writer->writeable() > 0;
}

template <typename T, typename U>
void FirModule<T, U>::process() {
    T* input = this->reader->getReadPointer();
    T* output = this->writer->getWritePointer();
    size_t size = std::min(this->reader->available() - filter->getLength(), this->writer->writeable());
    filter->apply(input, output, size);
    this->reader->advance(size);
    this->writer->advance(size);
}

namespace Csdr {
    template class Filter<complex<float>>;
    template class Filter<float>;

    template class SparseView<complex<float>>;
    template class SparseView<float>;

    template class FirFilter<complex<float>, float>;
    template class FirFilter<float, float>;
    template class FirFilter<complex<float>, complex<float>>;

    template class LowPassFilter<complex<float>>;
    template class LowPassFilter<float>;

    template class FirModule<complex<float>, float>;
    template class FirModule<float, float>;
    template class FirModule<complex<float>, complex<float>>;
}