#pragma once

#include "module.hpp"
#include "complex.hpp"

namespace Csdr {

    class AmDemod: public Module<complex<float>, float> {
        public:
            void process() override;
    };

}