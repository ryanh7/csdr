#include "power.hpp"

#include <cstring>
#include <utility>

using namespace Csdr;

Power::Power(unsigned int decimation, std::function<void(float)> callback): decimation(decimation), callback(std::move(callback)) {}

void Power::process(complex<float>* input, complex<float>* output) {
    float acc = 0;
    for (size_t i = 0; i < getLength(); i += decimation){
        acc += std::norm(input[i]);
    }
    float power = acc / ceilf((float) getLength() / decimation);
    callback(power);
    // pass data
    forwardData(input, output, power);
}

size_t Power::getLength() {
    return 1024;
}

void Power::forwardData(Csdr::complex<float> *input, Csdr::complex<float> *output, float power) {
    std::memcpy(output, input, getLength() * sizeof(complex<float>));
}

void Squelch::setSquelch(float squelchLevel) {
    this->squelchLevel = squelchLevel;
}

void Squelch::forwardData(complex<float> *input, complex<float> *output, float power) {
    if (squelchLevel == 0 || power >= squelchLevel) {
        Power::forwardData(input, output, power);
    } else {
        std::memset(output, 0, sizeof(complex<float>) * getLength());
    }
}