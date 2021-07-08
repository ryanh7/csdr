#include "fir.hpp"
#include "complex.hpp"
#include "fmv.h"

#include <cmath>
#include <cstring>

using namespace Csdr;

template<typename T>
FirFilter<T>::FirFilter(size_t length) {
    allocateTaps(length);
}

template <typename T>
FirFilter<T>::FirFilter(complex<float>* taps, size_t length): FirFilter(length) {
    // better to copy the taps to our memory since that is aligned
    std::memcpy(this->taps, taps, sizeof(complex<float>) * length);
}

template <typename T>
FirFilter<T>::FirFilter(float* taps, size_t length): FirFilter(length) {
    for (size_t i = 0; i < length; i++) {
        this->taps[i] = {taps[i], 0};
    }
}

template<typename T>
FirFilter<T>::~FirFilter() {
    free(taps);
}

template<typename T>
T FirFilter<T>::processSample(T *data, size_t index) {
    return processSample_fmv(data, index);
}

template <>
CSDR_TARGET_CLONES
float FirFilter<float>::processSample_fmv(float *data, size_t index) {
    float acc = 0;
    for (size_t ti = 0; ti < taps_length; ti++) {
        acc += data[index + ti] * taps[ti].i();
    }
    return acc;
}

template <typename T>
CSDR_TARGET_CLONES
T FirFilter<T>::processSample_fmv(T *data, size_t index) {
    T acc = 0;
    for (size_t ti = 0; ti < taps_length; ti++) {
        acc += data[index + ti] * taps[ti];
    }
    return acc;
}

template<typename T>
size_t FirFilter<T>::filterLength(float transition) {
    size_t result = 4.0 / transition;
    if (result % 2 == 0) result++; //number of symmetric FIR filter taps should be odd
    return result;
}

template<typename T>
size_t FirFilter<T>::getOverhead() {
    return taps_length;
}

template<typename T>
void FirFilter<T>::allocateTaps(size_t length) {
#ifdef NEON_OPTS
#define NEON_ALIGNMENT (4 * 4 * 2)
    size_t padded_taps_length = length;
    padded_taps_length = length + (NEON_ALIGNMENT / 4) -1 - ((length + (NEON_ALIGNMENT / 4) - 1) % (NEON_ALIGNMENT / 4));

    taps = (float*) malloc((padded_taps_length + NEON_ALIGNMENT) * sizeof(complex<float>));
    taps = (float*) ((((size_t) taps) + NEON_ALIGNMENT - 1) & ~(NEON_ALIGNMENT - 1));
    for (int i = 0; i < padded_taps_length - length; i++) taps[taps_length + i] = 0;
    taps_length = padded_taps_length;
#else
    taps = (complex<float>*) malloc(length * sizeof(complex<float>));
    taps_length = length;
#endif
}

template<typename T>
void FirFilter<T>::normalize() {
    //Normalize filter kernel
    float sum = 0;
    for (int i = 0; i < taps_length; i++) sum += std::abs(taps[i]);
    for (int i = 0; i < taps_length; i++) taps[i] = taps[i] / sum;
}

template <typename T>
FftFilter<T>* FirFilter<T>::fftTransform() {
    return FftFilter<T>::fromFirTaps(taps, taps_length);
}

template<typename T>
LowPassFilter<T>::LowPassFilter(float cutoff, float transition, Window* window): FirFilter<T>(FirFilter<T>::filterLength(transition)) {
    //Generates symmetric windowed sinc FIR filter real taps
    //  cutoff_rate is (cutoff frequency/sampling frequency)
    //Explanation at Chapter 16 of dspguide.com
    int middle = this->taps_length / 2;
    this->taps[middle] = 2 * M_PI * cutoff * window->kernel(0);
    for (int i = 1; i <= middle; i++)  {
        // by definition: assigning a scalar to a complex assigns the real part (our i) only,
        // with the imaginary part (our q) set to zero
        this->taps[middle - i] = this->taps[middle + i] =
                (sinf(2 * M_PI * cutoff * i) / i) * window->kernel((float) i / middle);
    }
    this->normalize();
}

template<typename T>
BandPassFilter<T>::BandPassFilter(float lowcut, float highcut, float transition, Window *window): LowPassFilter<T>((highcut - lowcut) / 2, transition, window) {
    //To generate a complex filter:
    //  1. we generate a real lowpass filter with a bandwidth of highcut-lowcut
    //  2. we shift the filter taps spectrally by multiplying with e^(j*w), so we get complex taps
    //(tnx HA5FT)

    float filter_center = (highcut + lowcut) / 2;
    float phase = 0, sinval, cosval;

    for(int i=0; i < this -> taps_length; i++) {
        sincosf(phase, &sinval, &cosval);
        phase += 2.0f * M_PI * filter_center;
        while (phase > 2 * M_PI) phase -= 2 * M_PI;
        while (phase < 0) phase += 2 * M_PI;
        this->taps[i] *= complex<float>(sinval, cosval);
    }
}

namespace Csdr {
    template class FirFilter<complex<float>>;
    template class FirFilter<float>;

    template class LowPassFilter<complex<float>>;
    template class LowPassFilter<float>;

    template class BandPassFilter<complex<float>>;
}