#include "fftfilter.hpp"

using namespace Csdr;

template <typename T>
FftFilter<T>::FftFilter(complex<float>* taps, size_t length): taps(taps), length(length) {}

template<typename T>
FftFilter<T>::~FftFilter() {
    free(taps);
}

template<typename T>
FftFilter<T> *FftFilter<T>::fromFirTaps(complex<float> *taps, size_t length) {
    return nullptr;
}

namespace Csdr {
    template class FftFilter<float>;
    template class FftFilter<complex<float>>;
}
