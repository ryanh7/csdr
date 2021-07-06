#pragma once

#include "module.hpp"
#include "complex.hpp"

namespace Csdr {

    template <typename T>
    class Gain: public AnyLengthModule<T, T> {
        public:
            explicit Gain(float gain);
            void process(T* input, T* output, size_t size) override;
        private:
            float gain;
    };

}