#pragma once

#include "module.hpp"
#include "ringbuffer.hpp"
#include "writer.hpp"

namespace Csdr {

    template <typename T>
    class Agc: public AnyLengthModule<T, T> {
        public:
            void process(T* input, T* output, size_t work_size) override;

            void setReference(float reference);
            void setAttack(float attack_rate);
            void setDecay(float decay_rate);
            void setMaxGain(float max_gain);
            void setInitialGain(float initial_gain);
            void setHangTime(unsigned long int hang_time);
        private:
            float abs(T in);
            bool isZero(T in);
            T scale(T in);

            // params
            // fast profile defaults
            float reference = 0.8;
            float attack_rate = 0.1;
            float decay_rate = 0.001;
            float max_gain = 65535;
            unsigned long int hang_time = 200;
            float gain_filter_alpha = 1.5;
            // state
            float gain = 1;
            float last_peak = 0;
            unsigned long int hang_counter = 0;
            float xk = 0;
            float vk = 0;
    };

}