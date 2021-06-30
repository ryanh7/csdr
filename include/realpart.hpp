#pragma once

#include "module.hpp"
#include "complex.hpp"

namespace Csdr {

    class Realpart: public AnyLengthModule<complex<float>, float> {
        public:
            void process(complex<float>* input, float* output, size_t size) override;
    };

}