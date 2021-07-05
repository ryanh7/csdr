#pragma once

#include <functional>
#include "module.hpp"
#include "complex.hpp"

namespace Csdr {

    class Power: public FixedLengthModule<complex<float>, complex<float>> {
        public:
            Power(unsigned int decimation, std::function<void(float)> callback);
            size_t getLength() override;
            void process(complex<float>* input, complex<float>* output) override;
        protected:
            // to bo overridden by the squelch implementation
            virtual void forwardData(complex<float>* input, complex<float>* output, float power);
        private:
            unsigned int decimation;
            std::function<void(float)> callback;
    };

    class Squelch: public Power {
        public:
            Squelch(unsigned int decimation, std::function<void(float)> callback): Power(decimation, callback) {}
            void setSquelch(float squelchLevel);
        protected:
            void forwardData(complex<float>* input, complex<float>* output, float power) override;
        private:
            float squelchLevel = 0.0f;
    };

}