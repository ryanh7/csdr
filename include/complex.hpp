#pragma once

#include <complex>

namespace Csdr {

    template <typename T>
    class complex: public std::complex<T> {
        public:
            complex(const T& i = T(), const T& q = T()): std::complex<T>(i, q) {}
            complex(std::complex<T> c): complex(c.real(), c.imag()) {}
            // in-phase
            T i() { return std::complex<T>::real(); }
            void i(T value) { std::complex<T>::real(value); }
            // quadrature
            T q() { return std::complex<T>::imag(); }
            void q(T value) { std::complex<T>::imag(value); }
    };

}