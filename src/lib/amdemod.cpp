#include "amdemod.hpp"

using namespace Csdr;

void AmDemod::process(complex<float>* input, float* output, size_t work_size) {
    for (size_t i = 0; i < work_size; i++) {
        output[i] = std::abs(input[i]);
    }
}