#include "logaveragepower.hpp"

#include <cstring>

using namespace Csdr;

LogAveragePower::LogAveragePower(unsigned int fftSize, unsigned int avgNumber, float add_db): fftSize(fftSize), avgNumber(avgNumber), add_db(add_db) {
    collector = (float*) malloc(sizeof(float) * fftSize);
    std::memset(collector, 0, sizeof(float) * fftSize);
}

LogAveragePower::LogAveragePower(unsigned int fftSize, unsigned int avgNumber): LogAveragePower(fftSize, avgNumber, 0.0) {}

LogAveragePower::~LogAveragePower() {
    free(collector);
}

void LogAveragePower::setAvgNumber(unsigned int avgNumber) {
    this->avgNumber = avgNumber;
}

bool LogAveragePower::canProcess() {
    std::lock_guard<std::mutex> lock(processMutex);
    return reader->available() > fftSize && writer->writeable() > fftSize;
}

void LogAveragePower::process() {
    std::lock_guard<std::mutex> lock(processMutex);
    complex<float>* input = reader->getReadPointer();
    for (int i = 0; i < fftSize; i++) {
        collector[i] += std::norm(input[i]);
    }
    reader->advance(fftSize);
    if (++collected == avgNumber) {
        float* output = writer->getWritePointer();
        float correction = add_db - 10.0 * log10(avgNumber);

        for (int i = 0; i < fftSize; i++) {
            output[i] = log10(collector[i]);
        }

        for (int i = 0; i < fftSize; i++) {
            output[i] = 10 * output[i] + correction;
        }

        writer->advance(fftSize);

        std::memset(collector, 0, sizeof(float) * fftSize);
        collected = 0;
    }
}
