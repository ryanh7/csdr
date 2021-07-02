#pragma once

#include "complex.hpp"

#include <cstdlib>

namespace Csdr {

    template <typename T>
    class Writer {
        public:
            virtual size_t write(T* data, size_t size) = 0;
            virtual size_t writeable() = 0;
            virtual T* getWritePointer()  = 0;
            virtual void advance(size_t how_much) = 0;
    };

    template <typename T>
    class StdoutWriter: public Writer<T> {
        public:
            StdoutWriter();
            StdoutWriter(size_t buffer_size);
            ~StdoutWriter();
            size_t write(T* data, size_t size) override;
            size_t writeable() override;
            T* getWritePointer() override;
            void advance(size_t how_much) override;
        private:
            size_t buffer_size;
            T* buffer;
    };

    template class StdoutWriter<char>;
    template class StdoutWriter<unsigned char>;
    template class StdoutWriter<short>;
    template class StdoutWriter<float>;
    template class StdoutWriter<complex<float>>;
}