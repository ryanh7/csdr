#pragma once

#include "window.hpp"
#include "complex.hpp"
#include "module.hpp"

namespace Csdr {

    template <typename T>
    class SparseView;

    template <typename T>
    class Filter {
        public:
            virtual T processSample(T* data, size_t index) = 0;
            SparseView<T> sparse(T* data);
            void apply(T* input, T* output, size_t size);
    };

    template <typename T>
    class SparseView {
        public:
            SparseView<T>(T* data, Filter<T>* filter);
            T operator[](size_t index);
        private:
            T* data;
            Filter<T>* filter;
    };

    template <typename T, typename U>
    class FirFilter: public Filter<T> {
        public:
            FirFilter(U* taps, unsigned int length);
            ~FirFilter();
            T processSample(T* data, size_t index) override;
            T processSample_fmv(T* data, size_t index);
            unsigned int getLength();
        protected:
            explicit FirFilter(unsigned int length);
            static unsigned int filterLength(float transition);
            void allocateTaps(unsigned int length);
            void normalize();
            U* taps;
            unsigned int taps_length;
    };

    template <typename T>
    class LowPassFilter: public FirFilter<T, float> {
        public:
            LowPassFilter(float cutoff, float transition, Window* window);
    };

    template <typename T, typename U>
    class FirModule: public Module<T, T> {
        public:
            explicit FirModule(FirFilter<T, U>* filter);
            bool canProcess() override;
            void process() override;
        private:
            FirFilter<T, U>* filter;
    };
}