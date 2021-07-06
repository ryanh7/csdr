#include "deemphasis.hpp"

#include <cmath>

using namespace Csdr;

WfmDeemphasis::WfmDeemphasis(unsigned int sampleRate, float tau): dt(1.0f / sampleRate), alpha(dt / tau + dt) {}

void WfmDeemphasis::process(float *input, float *output, size_t size) {
    /*
        typical time constant (tau) values:
        WFM transmission in USA: 75 us -> tau = 75e-6
        WFM transmission in EU:  50 us -> tau = 50e-6
        More info at: http://www.cliftonlaboratories.com/fm_receivers_and_de-emphasis.htm
        Simulate in octave: tau=75e-6; dt=1/48000; alpha = dt/(tau+dt); freqz([alpha],[1 -(1-alpha)])
    */
    if (std::isnan(last_output)) last_output = 0.0;
    for (int i = 0; i < size; i++) {
        output[i] = last_output = alpha * input[i] + (1 - alpha) * last_output; //this is the simplest IIR LPF
    }
}

