#ifndef CSDR_TYPES_H
#define CSDR_TYPES_H

typedef struct complexf_s { float i; float q; } complexf;

#ifdef USE_FFTW

#include <fftw3.h>

typedef struct fft_plan_s
{
	int size;
	void* input;
	void* output;
	fftwf_plan plan;
} FFT_PLAN_T;

#endif

#endif