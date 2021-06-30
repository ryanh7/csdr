#pragma once

#include "module.hpp"

namespace Csdr {

    class DcBlock: public FixedLengthModule<float, float> {
        public:
            void process(float* input, float* output) override;
            size_t getLength() override { return 1024; }
        private:
            float last_dc_level = 0.0;
    };

}