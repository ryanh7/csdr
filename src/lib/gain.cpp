#include "gain.hpp"

using namespace Csdr;


template<typename T>
Gain<T>::Gain(float gain): gain(gain) {}

template<typename T>
void Gain<T>::process(T *input, T *output, size_t size) {
    for (size_t i = 0; i < size; i++) {
        output[i] = static_cast<T>(input[i] * gain);
    }
}

namespace Csdr {
    template class Gain<float>;
    template class Gain<complex<float>>;
}