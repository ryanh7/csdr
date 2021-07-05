#include "adpcm.hpp"

using namespace Csdr;

unsigned char AdpcmCodec::encodeSample(short sample) {
    int diff = sample - previousValue;
    int step = _stepSizeTable[index];
    int deltaCode = 0;

    // Set sign bit
    if (diff < 0) {
        deltaCode = 8;
        diff = -diff;
    }

    // This is essentially deltaCode = (diff<<2)/step,
    // except the roundoff is handled differently.
    if (diff >= step) {
        deltaCode |= 4;
        diff -= step;
    }
    step >>= 1;
    if (diff >= step) {
        deltaCode |= 2;
        diff -= step;
    }
    step >>= 1;
    if (diff >= step) {
        deltaCode |= 1;
        diff -= step;
    }

    decodeSample(deltaCode);  // update state
    return deltaCode;
}

short AdpcmCodec::decodeSample(unsigned char deltaCode) {
    // Get the current step size
    int step = _stepSizeTable[index];

    // Construct the difference by scaling the current step size
    // This is approximately: difference = (deltaCode+.5)*step/4
    int difference = step>>3;
    if ( deltaCode & 1 ) difference += step>>2;
    if ( deltaCode & 2 ) difference += step>>1;
    if ( deltaCode & 4 ) difference += step;
    if ( deltaCode & 8 ) difference = -difference;

    // Build the new sample
    previousValue += difference;
    if (previousValue > 32767) previousValue = 32767;
    else if (previousValue < -32768) previousValue = -32768;

    // Update the step for the next sample
    index += indexAdjustTable[deltaCode];
    if (index < 0) index = 0;
    else if (index > 88) index = 88;

    return previousValue;
}

unsigned char AdpcmCodec::encodeSample(float input) {
    /*
     * TODO: 100 is a magic number here.
     * It only makes sense since the same constant is used in the OpenWebRX client-side javascript code.
     * SHRT_MAX does not work in it's place, so this is more than just a simple conversion.
     */
    return encodeSample((short) (input * 100));
}

void AdpcmCodec::reset() {
    previousValue = 0;
    index = 0;
}

AdpcmCoder::AdpcmCoder(): codec(new AdpcmCodec()) {}

AdpcmCoder::~AdpcmCoder() {
    delete codec;
}

bool AdpcmEncoder::canProcess() {
    return reader->available() >= 2 && writer->writeable() > 0;
}

void AdpcmEncoder::process() {
    short* input = reader->getReadPointer();
    unsigned char* output = writer->getWritePointer();
    size_t size = std::min(reader->available() / 2, writer->writeable());
    for(int i = 0; i < size; i++) {
        output[i] =
                codec->encodeSample(input[2 * i]) |
                codec->encodeSample(input[2 * i + 1]) << 4;
    }
    reader->advance(size * 2);
    writer->advance(size);
}

bool AdpcmDecoder::canProcess() {
    return reader->available() > 0 && writer->writeable() >= 2;
}

void AdpcmDecoder::process() {
    unsigned char* input = reader->getReadPointer();
    short* output = writer->getWritePointer();
    size_t size = std::min(reader->available(), writer->writeable() / 2);
    for (int i = 0; i < size; i++) {
        output[i * 2] = codec->decodeSample(input[i] & 0x0f);
        output[i * 2 + 1] = codec->decodeSample(input[i] >> 4);
    }
    reader->advance(size);
    writer->advance(size * 2);
}

FftAdpcmEncoder::FftAdpcmEncoder(unsigned int fftSize): fftSize(fftSize) {}

bool FftAdpcmEncoder::canProcess() {
    return reader->available() >= fftSize && (COMPRESS_FFT_PAD_N + writer->writeable()) / 2 > fftSize;
}

void FftAdpcmEncoder::process() {
    float* input = reader->getReadPointer();
    unsigned char* output = writer->getWritePointer();
    // FFT always starts with the codec default values
    codec->reset();
    for (int i = 0; i < COMPRESS_FFT_PAD_N / 2; i++) {
        output[i] =
                // the cast only serves to help the compiler decide which function to call
                codec->encodeSample(input[0]) |
                codec->encodeSample(input[0]) << 4;
    }
    output += (COMPRESS_FFT_PAD_N / 2);
    for (size_t i = 0; i < fftSize / 2; i++) {
        output[i] =
                codec->encodeSample(input[i * 2]) |
                codec->encodeSample(input[i * 2 + 1]) << 4;
    }
    reader->advance(fftSize);
    writer->advance((COMPRESS_FFT_PAD_N + fftSize) / 2);
}
