#pragma once

#include "module.hpp"
#include "complex.hpp"

namespace Csdr {

    class Shift {
        public:
            Shift(float rate);
            virtual void setRate(float rate);
        protected:
            float rate;
    };

    class ShiftAddfast: public Shift, public FixedLengthModule<complex<float>, complex<float>> {
        public:
            ShiftAddfast(float rate);
            void setRate(float rate) override;
            void process(complex<float>* input, complex<float>* output) override;
            void process_fmv(complex<float>* input, complex<float>* output, size_t size);
            size_t getLength() override { return 1024; }
        private:
            float starting_phase = 0.0;
            float dsin[4];
            float dcos[4];
            float phase_increment;
    };

}