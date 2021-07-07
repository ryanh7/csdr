#pragma once

#include "filter.hpp"
#include "complex.hpp"

namespace Csdr {

    template <typename T>
    class FftFilter: public Filter<T> {
        public:
            FftFilter(complex<float>* taps, size_t length);
            ~FftFilter();
            static FftFilter<T>* fromFirTaps(complex<float>* taps, size_t length);
        private:
            complex<float>* taps;
            size_t length;
    };

}