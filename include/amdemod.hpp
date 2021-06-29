#pragma once

#include "module.hpp"
#include "complex.hpp"

namespace Csdr {

    class AmDemod: public AnyLengthModule<complex<float>, float> {
        public:
            void process(complex<float>* input, float* output, size_t work_size) override;
    };

}