#include "limit.hpp"

Csdr::Limit::Limit(float maxAmplitude): maxAmplitude(maxAmplitude) {}

void Csdr::Limit::process(float *input, float *output, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (input[i] > maxAmplitude) {
            output[i] = maxAmplitude;
        } else if (input[i] < -maxAmplitude) {
            output[i] = -maxAmplitude;
        } else {
            output[i] = input[i];
        }
    }
}