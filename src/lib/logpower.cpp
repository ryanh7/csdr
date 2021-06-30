#include "logpower.hpp"

#include <iostream>

using namespace Csdr;

LogPower::LogPower(float add_db): add_db(add_db) {}

LogPower::LogPower(): LogPower(0.0) {}

void LogPower::process(complex<float>* input, float* output, size_t size) {
    for (int i = 0; i < size; i++) {
        output[i] = std::norm(input[i]);
    }

    for (int i = 0; i < size; i++) {
        output[i] = log10(output[i]);
    }

    for (int i = 0; i < size; i++) {
        output[i] = 10 * output[i] + add_db;
    }
}