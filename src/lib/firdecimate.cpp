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
    size_t samples = std::min((reader->available() - lowpass->getOverhead()) / decimation, writer->writeable());

    complex<float>* output = writer->getWritePointer();
    SparseView<complex<float>> sparseView = lowpass->sparse(reader->getReadPointer());
    for (size_t i = 0; i < samples; i++) {
        output[i] = sparseView[i * decimation];
    }
    reader->advance(samples * decimation);
    writer->advance(samples);
}

bool FirDecimate::canProcess() {
    size_t available = reader->available();
    size_t writeable = writer->writeable();
    size_t lpLen = lowpass->getOverhead();
    return available > lpLen && (available - lpLen) / decimation > 0 && writeable > 0;
}
