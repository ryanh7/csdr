#include "fmdemod.hpp"

#include <algorithm>

using namespace Csdr;

FmDemod::FmDemod(): AnyLengthModule() {
    temp_dq = (float*) malloc(sizeof(float) * buffer_size);
    temp_di = (float*) malloc(sizeof(float) * buffer_size);
}

FmDemod::~FmDemod() {
    free(temp_dq);
    free(temp_di);
}

void FmDemod::process(complex<float>* input, float* output, size_t work_size) {
    temp_dq[0] = input[0].q() - last_sample.q();
    for (int i = 1; i < work_size; i++) {
        temp_dq[i] = input[i].q() - input[i-1].q();
    }

    temp_di[0] = input[0].i() - last_sample.i();
    for (int i = 1; i < work_size; i++) {
        temp_di[i] = input[i].i() - input[i-1].i();
    }

    for (int i = 0; i < work_size; i++){
        output[i] = (input[i].i() * temp_dq[i] - input[i].q() * temp_di[i]);
    }

    for (int i = 0; i < work_size; i++) {
        temp_dq[i] = input[i].i() * input[i].i() + input[i].q() * input[i].q();
    }

    for (int i = 0; i < work_size; i++) {
        output[i] = temp_dq[i] ? fmdemod_quadri_K * output[i] / temp_dq[i] : 0;
    }

    last_sample = input[work_size - 1];
}