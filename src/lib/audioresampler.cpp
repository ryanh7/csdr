#include "audioresampler.hpp"

using namespace Csdr;

AudioResampler::AudioResampler(double rate):
    rate(rate)
{
    int error = 0;
    srcState = src_new(SRC_SINC_MEDIUM_QUALITY, 1, &error);
}

AudioResampler::AudioResampler(unsigned int inputRate, unsigned int outputRate):
    rate((double) outputRate / inputRate)
{}

AudioResampler::~AudioResampler() {
    src_delete(srcState);
}

bool AudioResampler::canProcess() {
    return reader->available() > 0 && writer->writeable() > 0;
}

void AudioResampler::process() {
    SRC_DATA data = {
        .data_in = reader->getReadPointer(),
        .data_out = writer->getWritePointer(),
        .input_frames = (long) reader->available(),
        .output_frames = (long) writer->writeable(),
        .end_of_input = 0,
        .src_ratio = rate
    };

    reader->advance(data.input_frames_used);
    reader->advance(data.output_frames_gen);

    src_process(srcState, &data);
}

