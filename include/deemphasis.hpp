#pragma once

#include "module.hpp"
#include "fir.hpp"

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

    class NfmDeephasis: public FirModule<float> {
        public:
            NfmDeephasis(unsigned int sampleRate);
        private:
            static FirFilter<float>* getFilter(unsigned int sampleRate);
    };

}