#include "converter.hpp"

#include <climits>

using namespace Csdr;

template <> void Converter<float, short>::process(float* input, short* output, size_t length) {
    for (int i = 0; i < length; i++) {
         output[i] = input[i] * SHRT_MAX;
    }
}

template <> void Converter<short, float>::process(short* input, float* output, size_t length) {
    for (int i = 0; i < length; i++) {
        output[i] = (float) input[i] / SHRT_MAX;
    }
}