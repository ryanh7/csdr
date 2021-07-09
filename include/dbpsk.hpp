#pragma once

#include "module.hpp"
#include "complex.hpp"

namespace Csdr {

    class DBPskDecoder: public AnyLengthModule<complex<float>, unsigned char> {
        public:
            void process(complex<float>* input, unsigned char* output, size_t size) override;
        private:
            float last_phase = 0.0f;
    };

}