#pragma once

#include "module.hpp"
#include "complex.hpp"

#define fmdemod_quadri_K 0.340447550238101026565118445432744920253753662109375

namespace Csdr {

    class FmDemod: public Module<complex<float>, float> {
        public:
            FmDemod();
            ~FmDemod();
            void process() override;
        private:
            complex<float> last_sample = {0, 0};
            size_t buffer_size = 1024;
            float* temp_dq;
            float* temp_di;
    };

}