#pragma once

#include <complex>

namespace Csdr {

    template <typename T>
    class complex: public std::complex<T> {
        public:
            complex(const T& re = T(), const T& im = T()): std::complex<T>(re, im) {}
            // in-phase
            T i() { return std::complex<T>::real(); }
            void i(T value) { std::complex<T>::real(value); }
            // quadrature
            T q() { return std::complex<T>::imag(); }
            void q(T value) { std::complex<T>::imag(value); }
    };

}