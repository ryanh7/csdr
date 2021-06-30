#pragma once

#include "module.hpp"
#include "complex.hpp"

namespace Csdr {

    class LogPower: public AnyLengthModule<complex<float>, float> {
        public:
            LogPower();
            LogPower(float add_db);
            void process(complex<float>* input, float* output, size_t size) override;
        private:
            float add_db = 0.0;
    };

}