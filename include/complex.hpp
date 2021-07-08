#pragma once

#include <complex>

namespace Csdr {

    template <typename T>
    class complex: public std::complex<T> {
        public:
            complex(const T& i = T(), const T& q = T()): std::complex<T>(i, q) {}
            complex(const std::complex<T>& c): complex(c.real(), c.imag()) {}
            complex(const complex& other): complex(other.i(), other.q()) {}
            // in-phase
            T i() const { return std::complex<T>::real(); }
            void i(T value) { std::complex<T>::real(value); }
            // quadrature
            T q() const { return std::complex<T>::imag(); }
            void q(T value) { std::complex<T>::imag(value); }
    };

}