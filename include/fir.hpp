#pragma once

#include "window.hpp"
#include "complex.hpp"
#include "module.hpp"
#include "filter.hpp"
#include "fftfilter.hpp"

namespace Csdr {

    template <typename T>
    class FirFilter: public SampleFilter<T> {
        public:
            FirFilter(complex<float>* taps, size_t length);
            FirFilter(float* taps, size_t length);
            ~FirFilter();
            T processSample(T* data, size_t index) override;
            T processSample_fmv(T* data, size_t index);
            size_t getOverhead() override;
            FftFilter<T>* fftTransform();
        protected:
            explicit FirFilter(size_t length);
            static size_t filterLength(float transition);
            void allocateTaps(size_t length);
            void normalize();
            complex<float>* taps;
            size_t taps_length;
    };

    template <typename T>
    class LowPassFilter: public FirFilter<T> {
        public:
            LowPassFilter(float cutoff, float transition, Window* window);
    };

    template <typename T>
    class BandPassFilter: public LowPassFilter<T> {
        public:
            BandPassFilter(float lowcut, float highcut, float transition, Window* window);
    };
}