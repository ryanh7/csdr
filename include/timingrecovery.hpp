#pragma once

#include "module.hpp"
#include "complex.hpp"

namespace Csdr {

    class TimingRecovery: public Module<complex<float>, complex<float>> {
        public:
            explicit TimingRecovery(unsigned int decimation, float loop_gain = 0.5f, float max_error = 2.0f, bool use_q = false);
            void process() override;
            bool canProcess() override;
        protected:
            virtual float getError() = 0;
            virtual int getErrorSign() = 0;
            float calculateError(int el_point_right_index, int el_point_left_index, int el_point_mid_index);
            unsigned int decimation;
            int correction_offset = 0;
        private:
            float loop_gain;
            float max_error;
            bool use_q;
    };

    class GardnerTimingRecovery: public TimingRecovery {
        public:
            using TimingRecovery::TimingRecovery;
        protected:
            float getError() override;
            int getErrorSign() override { return -1; }
    };

    class EarlyLateTimingRecovery: public TimingRecovery {
        public:
            using TimingRecovery::TimingRecovery;
        protected:
            float getError() override;
            int getErrorSign() override { return 1; }
        private:
            float earlylate_ratio = 0.25f;
    };

}