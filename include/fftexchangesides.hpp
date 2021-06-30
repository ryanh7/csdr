#pragma once

#include "module.hpp"

namespace Csdr {

    class FftExchangeSides: public FixedLengthModule<float, float> {
        public:
            FftExchangeSides(unsigned int fftSize);
            void process(float* input, float* output) override;
            size_t getLength() override;
        private:
            unsigned int fftSize;
    };

}