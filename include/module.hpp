#pragma once

#include "ringbuffer.hpp"
#include "writer.hpp"

#include <cstdint>

namespace Csdr {

    template <typename T, typename U>
    class Module {
        public:
            // TODO extract "Reader" interface and use here
            void setReader(RingbufferReader<T>* reader);
            void setWriter(Writer<U>* writer);
            virtual void process() = 0;
        protected:
            RingbufferReader<T>* reader;
            Writer<U>* writer;
    };

    template <typename T, typename U>
    class AnyLengthModule: public Module<T, U> {
        public:
            void process() override;
            virtual void process(T* input, U* output, size_t len) = 0;
        protected:
            virtual size_t maxLength() { return SIZE_MAX; }
    };

    template <typename T, typename U>
    class FixedLengthModule: public Module<T, U> {
        public:
            void process() override;
            virtual void process(T* input, U* output) = 0;
        protected:
            virtual size_t getLength() = 0;
    };

}