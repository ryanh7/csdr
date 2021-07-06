#pragma once

#include "module.hpp"

namespace Csdr {

    class WfmDeemphasis: public AnyLengthModule<float, float> {
        public:
            WfmDeemphasis(unsigned int sampleRate, float tau);
        protected:
            void process(float* input, float* output, size_t size) override;
        private:
            float dt;
            float alpha;
            float last_output = 0.0f;
    };

}