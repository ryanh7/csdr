#pragma once

#include "complex.hpp"

#include <cstdlib>
#include <mutex>

namespace Csdr {

    template <typename T>
    class Writer {
        public:
            virtual ~Writer() = default;
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
            size_t writeable() override;
            T* getWritePointer() override;
            void advance(size_t how_much) override;
        private:
            size_t buffer_size;
            T* buffer;
    };

    template <typename T>
    class VoidWriter: public Writer<T> {
        public:
            explicit VoidWriter(size_t buffer_size);
            VoidWriter();
            ~VoidWriter();
            size_t writeable() override;
            T* getWritePointer() override;
            void advance(__attribute__((unused)) size_t how_much) override {}
        private:
            size_t buffer_size;
            T* data;
    };

}