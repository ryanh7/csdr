#pragma once

#include "ringbuffer.hpp"
#include "writer.hpp"
#include "complex.hpp"

#include <cstdint>

namespace Csdr {

    template <typename T, typename U>
    class Module {
        public:
            virtual ~Module() = default;
            // TODO extract "Reader" interface and use here
            void setReader(RingbufferReader<T>* reader);
            void setWriter(Writer<U>* writer);
            virtual void process() = 0;
        protected:
            RingbufferReader<T>* reader;
            Writer<U>* writer;
    };

    template class Module<short, short>;
    template class Module<float, float>;
    template class Module<complex<float>, float>;
    template class Module<short, float>;
    template class Module<float, short>;
    template class Module<complex<float>, complex<float>>;

    template <typename T, typename U>
    class AnyLengthModule: public Module<T, U> {
        public:
            void process() override;
            virtual void process(T* input, U* output, size_t len) = 0;
        protected:
            virtual size_t maxLength() { return SIZE_MAX; }
    };

    template class AnyLengthModule<short, short>;
    template class AnyLengthModule<float, float>;
    template class AnyLengthModule<complex<float>, float>;
    template class AnyLengthModule<short, float>;
    template class AnyLengthModule<float, short>;
    template class AnyLengthModule<complex<float>, complex<float>>;

    template <typename T, typename U>
    class FixedLengthModule: public Module<T, U> {
        public:
            void process() override;
            virtual void process(T* input, U* output) = 0;
        protected:
            virtual size_t getLength() = 0;
    };

    template class FixedLengthModule<float, float>;
    template class FixedLengthModule<complex<float>, complex<float>>;
}