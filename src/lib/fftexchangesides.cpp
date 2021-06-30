#include "fftexchangesides.hpp"

#include <cstring>

using namespace Csdr;

FftExchangeSides::FftExchangeSides(unsigned int fftSize): fftSize(fftSize) {}

size_t FftExchangeSides::getLength() {
    return fftSize;
}

void FftExchangeSides::process(float* input, float* output) {
    unsigned int half = fftSize / 2;
    std::memcpy(output, input + half, sizeof(float) * half);
    std::memcpy(output + half, input, sizeof(float) * half);
}