#include "firdecimate.hpp"

using namespace Csdr;

FirDecimate::FirDecimate(unsigned int decimation, float transitionBandwidth, Window* window):
    decimation(decimation),
    lowpass(new LowPassFilter<complex<float>>(0.5f / (float) decimation, transitionBandwidth, window))
{}

FirDecimate::~FirDecimate() {
    delete lowpass;
}

void FirDecimate::process() {
    size_t available;
    while (reader->available() > lowpass->getLength() && (available = ((reader->available() - lowpass->getLength()) / decimation)) > 0) {
        complex<float>* output = writer->getWritePointer();
        SparseView<complex<float>> sparseView = lowpass->sparse(reader->getReadPointer());
        for (size_t i = 0; i < available; i++) {
            output[i] = sparseView[i * decimation];
        }
        reader->advance(available * decimation);
        writer->advance(available);
    }
}