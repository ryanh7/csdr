#include "fmdemod.hpp"

#include <algorithm>

using namespace Csdr;

FmDemod::FmDemod(): Module() {
    temp_dq = (float*) malloc(sizeof(float) * buffer_size);
    temp_di = (float*) malloc(sizeof(float) * buffer_size);
}

FmDemod::~FmDemod() {
    free(temp_dq);
    free(temp_di);
}

void FmDemod::process() {
    size_t available;
    while (available = reader->available()) {
        size_t input_size = std::min({buffer_size, available, writer->writeable()});
        complex<float>* input = reader->getReadPointer();
        float* output = writer->getWritePointer();

        temp_dq[0] = input[0].q() - last_sample.q();
        for (int i = 1; i < input_size; i++) {
            temp_dq[i] = input[i].q() - input[i-1].q();
        }

        temp_di[0] = input[0].i() - last_sample.i();
        for (int i = 1; i < input_size; i++) {
            temp_di[i] = input[i].i() - input[i-1].i();
        }

        for (int i = 0; i < input_size; i++){
            output[i] = (input[i].i() * temp_dq[i] - input[i].q() * temp_di[i]);
        }

        for (int i = 0; i < input_size; i++) {
            temp_dq[i] = input[i].i() * input[i].i() + input[i].q() * input[i].q();
        }

        for (int i = 0; i < input_size; i++) {
            output[i] = temp_dq[i] ? fmdemod_quadri_K * output[i] / temp_dq[i] : 0;
        }

        last_sample = input[input_size-1];

        reader->advance(input_size);
        writer->advance(input_size);
    }
}