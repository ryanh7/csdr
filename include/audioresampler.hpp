#pragma once

#include "module.hpp"

#include <samplerate.h>

namespace Csdr {

    class AudioResampler: public Module<float, float> {
        public:
            AudioResampler(unsigned int inputRate, unsigned int outputRate);
            explicit AudioResampler(double rate);
            ~AudioResampler() override;
            bool canProcess() override;
            void process() override;
        private:
            double rate;
            SRC_STATE* srcState;
    };

}