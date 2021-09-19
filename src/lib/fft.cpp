/*
Copyright (c) 2021 Jakob Ketterl <jakob.ketterl@gmx.de>

This file is part of libcsdr.

libcsdr is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "fft.hpp"

#include <cstring>

using namespace Csdr;

Fft::Fft(unsigned int fftSize, unsigned int everyNSamples, Window* window): fftSize(fftSize), everyNSamples(everyNSamples) {
    windowed = (complex<float>*) malloc(sizeof(complex<float>) * fftSize);
    output_buffer = (complex<float>*) malloc(sizeof(complex<float>) * fftSize);
    plan = fftwf_plan_dft_1d(fftSize, (fftwf_complex*) windowed, (fftwf_complex*) output_buffer, FFTW_FORWARD, FFTW_ESTIMATE);
    this->window = window->precalculate(fftSize);
}

Fft::~Fft() {
    free(windowed);
    free(output_buffer);
    delete window;
    fftwf_destroy_plan(plan);
}

bool Fft::canProcess() {
    std::lock_guard<std::mutex> lock(this->processMutex);
    return std::min(reader->available(), writer->writeable()) > std::max(fftSize, everyNSamples);
}

void Fft::process() {
    std::lock_guard<std::mutex> lock(processMutex);
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

void Fft::setEveryNSamples(unsigned int everyNSamples) {
    this->everyNSamples = everyNSamples;
}
