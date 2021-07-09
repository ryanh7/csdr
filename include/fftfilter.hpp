#pragma once

#include "filter.hpp"
#include "complex.hpp"
#include "window.hpp"

#include <fftw3.h>

namespace Csdr {

    template <typename T>
    class FftFilter: public Filter<T> {
        public:
            FftFilter(size_t fftSize, complex<float>* taps, size_t taps_length);
            ~FftFilter();
            size_t apply(T* input, T* output, size_t size) override;
            size_t getMinProcessingSize() override { return inputSize; }
        protected:
            FftFilter(size_t fftSize);
            static size_t filterLength(float transition);
            static size_t getFftSize(size_t taps_length);
            complex<float>* taps;
            size_t taps_length;
            size_t fftSize;
            size_t inputSize;
        private:
            fftwf_complex* forwardInput;
            fftwf_complex* forwardOutput;
            fftwf_plan forwardPlan;
            fftwf_complex* inverseInput;
            fftwf_complex* inverseOutput;
            fftwf_plan inversePlan;
            T* overlap;
    };

    class FftBandPassFilter: public FftFilter<complex<float>> {
        public:
            FftBandPassFilter(float lowcut, float highcut, float transition, Window* window);
    };

}