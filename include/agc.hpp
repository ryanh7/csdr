#pragma once

#include "module.hpp"
#include "ringbuffer.hpp"
#include "writer.hpp"

namespace Csdr {

    class UntypedAgc {
        public:
            virtual void setReference(float reference) = 0;
            virtual void setAttack(float attack_rate) = 0;
            virtual void setDecay(float decay_rate) = 0;
            virtual void setMaxGain(float max_gain) = 0;
            virtual void setInitialGain(float initial_gain) = 0;
            virtual void setHangTime(unsigned long int hang_time) = 0;
    };

    template <typename T>
    class Agc: public UntypedAgc, public AnyLengthModule<T, T> {
        public:
            void process(T* input, T* output, size_t work_size) override;

            void setReference(float reference) override;
            void setAttack(float attack_rate) override;
            void setDecay(float decay_rate) override;
            void setMaxGain(float max_gain) override;
            void setInitialGain(float initial_gain) override;
            void setHangTime(unsigned long int hang_time) override;
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