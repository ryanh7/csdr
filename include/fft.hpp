#pragma once

#include "module.hpp"
#include "complex.hpp"
#include "window.hpp"

#include <fftw3.h>

namespace Csdr {

    class Fft: public Module<complex<float>, complex<float>> {
        public:
            Fft(unsigned int fftSize, unsigned int everyNSamples, Window* window);
            Fft(unsigned int fftSize, unsigned int everyNSamples);
            ~Fft();
            void process() override;
        private:
            unsigned int fftSize;
            unsigned int everyNSamples;
            PrecalculatedWindow* window;
            fftwf_plan plan;
            complex<float>* windowed;
            complex<float>* output_buffer;
    };

}