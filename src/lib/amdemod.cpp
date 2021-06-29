#include "amdemod.hpp"

using namespace Csdr;

void AmDemod::process() {
    size_t available;
    while (available = reader->available()) {
        float* output = (float*) malloc(sizeof(float) * available);
        complex<float>* input = reader->getReadPointer();

        for (size_t i = 0; i < available; i++) {
            output[i] = std::abs(input[i]);
        }

        reader->advance(available);
        writer->write(output, available);
        free(output);
    }
}