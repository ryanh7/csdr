#include "realpart.hpp"

using namespace Csdr;

void Realpart::process(complex<float>* input, float* output, size_t size) {
    for (size_t i = 0; i < size; i++) {
        output[i] = input[i].i();
    }
}