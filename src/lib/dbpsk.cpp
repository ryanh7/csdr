#include "dbpsk.hpp"
#include <cmath>

using namespace Csdr;

void DBPskDecoder::process(complex<float> *input, unsigned char *output, size_t size) {
    for (size_t i = 0; i < size; i++) {
        float phase = std::arg(input[i]);
        if (std::isnan(phase)) phase = 0.0f;
        float dphase = phase - last_phase;
        while (dphase < -M_PI) dphase += 2 * M_PI;
        while (dphase >= M_PI) dphase -= 2 * M_PI;
        if (dphase > (M_PI / 2) || dphase < (-M_PI / 2)) {
            output[i] = 0;
        } else {
            output[i] = 1;
        }
        last_phase = phase;
    }
}
