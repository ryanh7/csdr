#include "fft.hpp"

#include <cstring>

using namespace Csdr;

Fft::Fft(unsigned int fftSize, unsigned int everyNSamples, Window* window): fftSize(fftSize), everyNSamples(everyNSamples) {
    windowed = (complex<float>*) malloc(sizeof(complex<float>) * fftSize);
    output_buffer = (complex<float>*) malloc(sizeof(complex<float>) * fftSize);
    plan = fftwf_plan_dft_1d(fftSize, (fftwf_complex*) windowed, (fftwf_complex*) output_buffer, FFTW_FORWARD, FFTW_ESTIMATE);
    this->window = window->precalculate(fftSize);
    delete window;
}

Fft::~Fft() {
    if (window != nullptr) delete window;
    fftwf_destroy_plan(plan);
}

bool Fft::canProcess() {
    return std::min(reader->available(), writer->writeable()) > std::max(fftSize, everyNSamples);
}

void Fft::process() {
    if (window != nullptr) {
        window->apply(reader->getReadPointer(), windowed, fftSize);
    } else {
        memcpy(windowed, reader->getReadPointer(), fftSize);
    }
    fftwf_execute(plan);
    std::memcpy(writer->getWritePointer(), output_buffer, sizeof(complex<float>) * fftSize);
    this->reader->advance(everyNSamples);
    this->writer->advance(fftSize);
}
