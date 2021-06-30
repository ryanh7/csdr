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

Fft::Fft(unsigned int fftSize, unsigned int everyNSamples): Fft(fftSize, everyNSamples, new HammingWindow()) {}

Fft::~Fft() {
    delete window;
    fftwf_destroy_plan(plan);
}

void Fft::process() {
    size_t available;
    while ((available = reader->available()) > std::max(fftSize, everyNSamples)) {
        window->apply(reader->getReadPointer(), windowed, fftSize);
        fftwf_execute(plan);
        std::memcpy(writer->getWritePointer(), output_buffer, sizeof(complex<float>) * fftSize);
        this->reader->advance(everyNSamples);
        this->writer->advance(fftSize);
    }
}
