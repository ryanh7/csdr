#pragma once

#include "module.hpp"
#include "complex.hpp"

namespace Csdr {

    class LogAveragePower: public Module<complex<float>, float> {
        public:
            LogAveragePower(unsigned int fftSize, unsigned int avgNumber, float add_db);
            LogAveragePower(unsigned int fftSize, unsigned int avgNumber);
            ~LogAveragePower();
            void process() override;
        private:
            float* collector;
            unsigned int collected = 0;
            unsigned int fftSize;
            unsigned int avgNumber;
            float add_db;
    };

}