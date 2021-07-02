#pragma once

#include "module.hpp"
#include "complex.hpp"
#include "window.hpp"
#include "fir.hpp"

namespace Csdr {

    class FirDecimate: public Module<complex<float>, complex<float>> {
        public:
            FirDecimate(unsigned int decimation, float transitionBandwidth, Window* window);
            ~FirDecimate() override;
            void process() override;
        private:
            unsigned int decimation;
            LowPassFilter<complex<float>>* lowpass;
    };

}