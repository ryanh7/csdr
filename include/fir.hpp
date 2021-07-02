#pragma once

#include "window.hpp"
#include "complex.hpp"

namespace Csdr {

    template <typename T>
    class SparseView;

    template <typename T>
    class Filter {
        public:
            virtual T processSample(T* data, size_t index) = 0;
            SparseView<T> sparse(T* data);
    };

    template class Filter<complex<float>>;

    template <typename T>
    class SparseView {
        public:
            SparseView<T>(T* data, Filter<T>* filter);
            T operator[](size_t index);
        private:
            T* data;
            Filter<T>* filter;
    };

    template class SparseView<complex<float>>;

    template <typename T>
    class FirFilter: public Filter<T> {
        public:
            ~FirFilter();
            T processSample(T* data, size_t index) override;
            T processSample_fmv(T* data, size_t index);
            unsigned int getLength();
        protected:
            explicit FirFilter(unsigned int length);
            static unsigned int filterLength(float transition);
            void allocateTaps(unsigned int length);
            void normalize();
            float* taps;
            unsigned int taps_length;
    };

    template class FirFilter<complex<float>>;

    template <typename T>
    class LowPassFilter: public FirFilter<T> {
        public:
            LowPassFilter(float cutoff, float transition, Window* window);
    };

    template class LowPassFilter<complex<float>>;
}