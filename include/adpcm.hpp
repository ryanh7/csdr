#pragma once

#include "module.hpp"

namespace Csdr {

    class AdpcmCodec {
        public:
            unsigned char encodeSample(short sample);
            short decodeSample(unsigned char deltaCode);
            // for FFT use only
            unsigned char encodeSample(float input);
            void reset();
        private:
            int index = 0;         // Index into step size table
            int previousValue = 0; // Most recent sample value
            const int _stepSizeTable[89] = {
                    7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34,
                    37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143,
                    157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494,
                    544, 598, 658, 724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552,
                    1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327, 3660, 4026,
                    4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442,
                    11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623,
                    27086, 29794, 32767
            };
            const int indexAdjustTable[16] = {
                    -1, -1, -1, -1,  // +0 - +3, decrease the step size
                    2, 4, 6, 8,      // +4 - +7, increase the step size
                    -1, -1, -1, -1,  // -0 - -3, decrease the step size
                    2, 4, 6, 8,      // -4 - -7, increase the step size
            };
    };

    class AdpcmCoder {
        protected:
            AdpcmCoder();
            ~AdpcmCoder();
            AdpcmCodec* codec;
    };

    class AdpcmEncoder: private AdpcmCoder, public Module<short, unsigned char> {
        public:
            bool canProcess() override;
            void process() override;
    };

    class AdpcmDecoder: private AdpcmCoder, public Module<unsigned char, short> {
        public:
            bool canProcess() override;
            void process() override;
    };

//We will pad the FFT at the beginning, with the first value of the input data, COMPRESS_FFT_PAD_N times.
//No, this is not advanced DSP, just the ADPCM codec produces some gabarge samples at the beginning,
//so we just add data to become garbage and get skipped.
//COMPRESS_FFT_PAD_N should be even.
#define COMPRESS_FFT_PAD_N 10

    // ADPCM encoding for the FFT works a bit different
    class FftAdpcmEncoder: private AdpcmCoder, public Module<float, unsigned char> {
        public:
            explicit FftAdpcmEncoder(unsigned int fftSize);
            bool canProcess() override;
            void process() override;
        private:
            unsigned int fftSize;
    };

}