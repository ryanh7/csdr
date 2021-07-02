#pragma once

#include "reader.hpp"
#include "writer.hpp"
#include "complex.hpp"

#include <cstdint>

namespace Csdr {

    template <typename T, typename U>
    class Module {
        public:
            virtual ~Module() = default;
            void setReader(Reader<T>* reader);
            void setWriter(Writer<U>* writer);
            virtual void process() = 0;
        protected:
            Reader<T>* reader;
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