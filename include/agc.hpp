#pragma once

#include "module.hpp"
#include "ringbuffer.hpp"
#include "writer.hpp"

namespace Csdr {

    template <typename T>
    class Agc: public Module<T, T> {
        public:
            Agc(RingbufferReader<T>* in, Writer<T>* out): Module<T, T>(in, out) {}
            void process() override;

            void setReference(float reference);
            void setAttack(float attack_rate);
            void setDecay(float decay_rate);
            void setMaxGain(float max_gain);
            void setHangTime(unsigned long int hang_time);
        private:
            T min();
            T max();
            T abs(T in);

            // params
            // slow profile defaults
            float reference = 0.8;
            float attack_rate = 0.01;
            float decay_rate = 0.0001;
            float max_gain = 65535;
            unsigned long int hang_time = 600;
            short attack_wait_time = 0;
            float gain_filter_alpha = 1.5;
            // state
            // TODO setter for initial gain
            float last_gain = 1;
            unsigned long int hang_counter = 0;
            short attack_wait_counter = 0;
            float xk = 0;
            float vk = 0;
    };

}