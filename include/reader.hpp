#pragma once

#include "complex.hpp"

#include <cstdlib>

namespace Csdr {

    template <typename T>
    class Reader {
        public:
            virtual ~Reader() = default;
            virtual size_t available() = 0;
            virtual T* getReadPointer() = 0;
            virtual void advance(size_t how_much) = 0;
            virtual void wait() = 0;
    };

    template <typename T>
    class MemoryReader: public Reader<T> {
        public:
            MemoryReader(T* data, size_t size);
            size_t available() override;
            T* getReadPointer() override;
            void advance(size_t how_much) override;
            void wait() override;
            void rewind();
        private:
            T* data;
            size_t size;
            size_t read_pos = 0;
    };

}