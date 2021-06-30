#pragma once

#include "module.hpp"

namespace Csdr {

    template <typename T, typename U>
    class Converter: public AnyLengthModule<T, U> {
        public:
            void process(T* input, U* output, size_t length) override;
    };

}