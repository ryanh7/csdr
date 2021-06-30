#include "dcblock.hpp"

using namespace Csdr;

//this DC block filter does moving average block-by-block.
//this is the most computationally efficient
//input and output buffer is allowed to be the same
//http://www.digitalsignallabs.com/dcblock.pdf

void DcBlock::process(float* input, float* output) {
    float avg = 0.0;
    size_t input_size = getLength();
    for(int i = 0; i < input_size; i++) {
        avg += input[i];
    }
    avg /= input_size;

    float avgdiff = avg - last_dc_level;
    last_dc_level = avg;

    //DC removal level will change lineraly from last_dc_level to avg.
    for(int i = 0; i < input_size; i++) {
        float dc_removal_level = last_dc_level + avgdiff * ((float) i / input_size);
        output[i] = input[i] - dc_removal_level;
    }
}