#include "amdemod.hpp"

using namespace Csdr;

void AmDemod::process() {
    size_t available;
    while (available = reader->available()) {
        size_t work_size = std::min(available, writer->writeable());
        complex<float>* input = reader->getReadPointer();
        float* output = writer->getWritePointer();

        for (size_t i = 0; i < work_size; i++) {
            output[i] = std::abs(input[i]);
        }

        reader->advance(work_size);
        writer->advance(work_size);
    }
}