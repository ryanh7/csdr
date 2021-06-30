#include "window.hpp"
#include "complex.hpp"
#include <cmath>

using namespace Csdr;

template<> void Window::apply<float>(float* input, float* output, size_t size) {
    for (size_t i = 0; i <size; i++) {
        float rate = (float) i / (size - 1);
        output[i] = input[i] * kernel(2.0 * rate + 1.0);
    }
}

template<> void Window::apply<complex<float>>(complex<float>* input, complex<float>* output, size_t size) {
    for (size_t i = 0; i < size; i++) {
        float rate = (float) i / (size - 1);
        output[i].i(input[i].i() * kernel(2.0 * rate + 1.0));
        output[i].q(input[i].q() * kernel(2.0 * rate + 1.0));
    }
}

PrecalculatedWindow* Window::precalculate(size_t size) {
    float *windowt;
    windowt = (float*) malloc(sizeof(float) * size);
    for (size_t i = 0; i < size; i++) {
        float rate = (float) i / (size-1);
        windowt[i] = kernel(2.0 * rate + 1.0);
    }
    return new PrecalculatedWindow(windowt, size);
}

PrecalculatedWindow::PrecalculatedWindow(float* windowt, size_t size): windowt(windowt), size(size) {}

PrecalculatedWindow::~PrecalculatedWindow() {
    free(windowt);
}

template<> void PrecalculatedWindow::apply<float>(float* input, float* output, size_t size) {
	for (size_t i = 0; i < size; i++) {
		output[i] = input[i] * windowt[i];
	}
}

template<> void PrecalculatedWindow::apply<complex<float>>(complex<float>* input, complex<float>* output, size_t size) {
    for (size_t i = 0; i < size; i++) {
        output[i].i(input[i].i() * windowt[i]);
        output[i].q(input[i].q() * windowt[i]);
    }
}

float BoxcarWindow::kernel(float rate) {
    //"Dummy" window kernel, do not use; an unwindowed FIR filter may have bad frequency response
    return 1.0;
}

float BlackmanWindow::kernel(float rate) {
    //Explanation at Chapter 16 of dspguide.com, page 2
    //Blackman window has better stopband attentuation and passband ripple than Hamming, but it has slower rolloff.
    rate = 0.5 + rate / 2;
    return 0.42 - 0.5 * cos(2 * M_PI * rate) + 0.08 * cos(4 * M_PI * rate);
}

float HammingWindow::kernel(float rate) {
    //Explanation at Chapter 16 of dspguide.com, page 2
    //Hamming window has worse stopband attentuation and passband ripple than Blackman, but it has faster rolloff.
    rate = 0.5 + rate / 2;
    return 0.54 - 0.46 * cos(2 * M_PI * rate);
}
