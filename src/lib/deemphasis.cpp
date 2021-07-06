#include "deemphasis.hpp"
#include "predefined.h"

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

NfmDeephasis::NfmDeephasis(unsigned int sampleRate): FirModule<float>(getFilter(sampleRate)) {}

FirFilter<float>* NfmDeephasis::getFilter(unsigned int sampleRate) {
    switch (sampleRate) {
        // we only cover selected sample rates. see predefined.h for details.
        case 8000:
            return new FirFilter<float>(deemphasis_nfm_predefined_fir_8000, 79);
        case 11025:
            return new FirFilter<float>(deemphasis_nfm_predefined_fir_11025, 79);
        case 12000:
            return new FirFilter<float>(deemphasis_nfm_predefined_fir_44100, 79);
        case 44100:
            return new FirFilter<float>(deemphasis_nfm_predefined_fir_44100, 199);
        case 48000:
            return new FirFilter<float>(deemphasis_nfm_predefined_fir_48000, 199);
        default:
            throw std::runtime_error("invalid sample rate");
    }
}