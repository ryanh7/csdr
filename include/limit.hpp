#pragma once

#include "module.hpp"

namespace Csdr {

    class Limit: public AnyLengthModule<float, float> {
        public:
            explicit Limit(float maxAmplitude);
            void process(float* input, float* output, size_t size) override;
        private:
            float maxAmplitude;
    };

}