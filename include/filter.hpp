#pragma once

#include "module.hpp"

#include <cstdlib>

namespace Csdr {

    template <typename T>
    class SparseView;

    template <typename T>
    class Filter {
        public:
            virtual T processSample(T* data, size_t index) = 0;
            SparseView<T> sparse(T* data);
            void apply(T* input, T* output, size_t size);
            virtual size_t getLength() { return 0; };
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

    template <typename T>
    class FilterModule: public Module<T, T> {
        public:
            explicit FilterModule(Filter<T>* filter);
            bool canProcess() override;
            void process() override;
            void setFilter(Filter<T>* filter);
        private:
            Filter<T>* filter;
    };
}