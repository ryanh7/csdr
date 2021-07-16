#pragma once

#include "complex.hpp"

#include <cstdlib>

namespace Csdr {

    // container class for template-agnostic storage
    class UntypedReader {
        public:
            virtual ~UntypedReader() = default;
            virtual size_t available() = 0;
            virtual void advance(size_t how_much) = 0;
            virtual void wait() = 0;
            virtual void unblock() = 0;
    };

    template <typename T>
    class Reader: public UntypedReader {
        public:
            virtual T* getReadPointer() = 0;
    };

    template <typename T>
    class MemoryReader: public Reader<T> {
        public:
            MemoryReader(T* data, size_t size);
            size_t available() override;
            T* getReadPointer() override;
            void advance(size_t how_much) override;
            void wait() override;
            void unblock() override {}
            void rewind();
        private:
            T* data;
            size_t size;
            size_t read_pos = 0;
    };

}