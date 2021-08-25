#include "audioresampler.hpp"

using namespace Csdr;

AudioResampler::AudioResampler(double rate):
    rate(rate)
{
    int error = 0;
    srcState = src_new(SRC_SINC_MEDIUM_QUALITY, 1, &error);
}

AudioResampler::AudioResampler(unsigned int inputRate, unsigned int outputRate):
    AudioResampler((double) outputRate / inputRate)
{}

AudioResampler::~AudioResampler() {
    src_delete(srcState);
}

bool AudioResampler::canProcess() {
    std::lock_guard<std::mutex> lock(this->processMutex);
    return reader->available() > 0 && writer->writeable() > 0;
}

void AudioResampler::process() {
    std::lock_guard<std::mutex> lock(processMutex);
    SRC_DATA data = {
        .data_in = reader->getReadPointer(),
        .data_out = writer->getWritePointer(),
        .input_frames = (long) reader->available(),
        .output_frames = (long) writer->writeable(),
        .end_of_input = 0,
        .src_ratio = rate
    };

    src_process(srcState, &data);

    reader->advance(data.input_frames_used);
    writer->advance(data.output_frames_gen);
}

