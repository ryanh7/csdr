#pragma once

#include <cstdlib>

namespace Csdr {

    template <typename T>
    class Writer {
        public:
            virtual size_t write(T* data, size_t size) = 0;
    };

    template <typename T>
    class StdoutWriter: public Writer<T> {
        public:
            size_t write(T* data, size_t size) override;
    };

}