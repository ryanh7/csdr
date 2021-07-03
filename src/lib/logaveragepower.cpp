#include "logaveragepower.hpp"

#include <cstring>

using namespace Csdr;

LogAveragePower::LogAveragePower(unsigned int fftSize, unsigned int avgNumber, float add_db): fftSize(fftSize), avgNumber(avgNumber), add_db(add_db - 10.0 * log10(avgNumber)) {
    collector = (float*) malloc(sizeof(float) * fftSize);
    std::memset(collector, 0, sizeof(float) * fftSize);
}

LogAveragePower::LogAveragePower(unsigned int fftSize, unsigned int avgNumber): LogAveragePower(fftSize, avgNumber, 0.0) {}

LogAveragePower::~LogAveragePower() {
    free(collector);
}

bool LogAveragePower::canProcess() {
    return reader->available() > fftSize && writer->writeable() > fftSize;
}

void LogAveragePower::process() {
    complex<float>* input = reader->getReadPointer();
    for (int i = 0; i < fftSize; i++) {
        collector[i] += std::norm(input[i]);
    }
    reader->advance(fftSize);
    if (++collected == avgNumber) {
        float* output = writer->getWritePointer();

        for (int i = 0; i < fftSize; i++) {
            output[i] = log10(collector[i]);
        }

        for (int i = 0; i < fftSize; i++) {
            output[i] = 10 * output[i] + add_db;
        }

        writer->advance(fftSize);

        std::memset(collector, 0, sizeof(float) * fftSize);
        collected = 0;
    }
}
