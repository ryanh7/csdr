#pragma once

#include "module.hpp"

#include <cstdlib>

namespace Csdr {

    template <typename T>
    class Filter {
        public:
            virtual ~Filter() = default;
            virtual size_t apply(T* input, T* output, size_t size) = 0;
            virtual size_t getMinProcessingSize() { return 0; }
            virtual size_t getOverhead() { return 0; };
    };

    template <typename T>
    class SparseView;

    template <typename T>
    class SampleFilter: public Filter<T> {
        public:
            SparseView<T> sparse(T* data);
            virtual T processSample(T* data, size_t index) = 0;
            size_t apply(T* input, T* output, size_t size) override;
    };

    template <typename T>
    class SparseView {
        public:
            SparseView<T>(T* data, SampleFilter<T>* filter);
            T operator[](size_t index);
        private:
            T* data;
            SampleFilter<T>* filter;
    };

    template <typename T>
    class FilterModule: public Module<T, T> {
        public:
            explicit FilterModule(Filter<T>* filter);
            ~FilterModule() override;
            bool canProcess() override;
            void process() override;
            void setFilter(Filter<T>* filter);
        private:
            Filter<T>* filter;
    };
}