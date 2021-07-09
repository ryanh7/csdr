#pragma once

#include "window.hpp"
#include "complex.hpp"
#include "module.hpp"
#include "filter.hpp"
#include "fftfilter.hpp"

namespace Csdr {

    template <typename T, typename U>
    class FirFilter: public SampleFilter<T> {
        public:
            FirFilter(U* taps, size_t length);
            ~FirFilter();
            T processSample(T* data, size_t index) override;
            T processSample_fmv(T* data, size_t index);
            size_t getOverhead() override;
        protected:
            explicit FirFilter(size_t length);
            static size_t filterLength(float transition);
            void allocateTaps(size_t length);
            U* taps;
            size_t taps_length;
    };

    template <typename T>
    class TapGenerator {
        public:
            TapGenerator(Window* window);
            virtual T* generateTaps(size_t length) = 0;
            complex<float>* fftTransform(size_t length, size_t fftSize);
        protected:
            void normalize(T* taps, size_t length);
            Window* window;
    };

    class LowPassTapGenerator: public TapGenerator<float> {
        public:
            LowPassTapGenerator(float cutoff, Window* window);
            float* generateTaps(size_t length) override;
        private:
            float cutoff;
    };

    template <typename T>
    class LowPassFilter: public FirFilter<T, float> {
        public:
            LowPassFilter(float cutoff, float transition, Window* window);
    };

    class BandPassTapGenerator: public TapGenerator<complex<float>> {
        public:
            BandPassTapGenerator(float lowcut, float highcut, Window* window);
            complex<float>* generateTaps(size_t length) override;
        private:
            float lowcut;
            float highcut;
    };

    template <typename T>
    class BandPassFilter: public FirFilter<T, complex<float>> {
        public:
            BandPassFilter(float lowcut, float highcut, float transition, Window* window);
    };
}