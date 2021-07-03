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
            virtual bool canProcess() = 0;
            virtual void process() = 0;
        protected:
            Reader<T>* reader;
            Writer<U>* writer;
    };

    template <typename T, typename U>
    class AnyLengthModule: public Module<T, U> {
        public:
            bool canProcess() override;
            void process() override;
        protected:
            virtual void process(T* input, U* output, size_t len) = 0;
            virtual size_t maxLength() { return SIZE_MAX; }
            size_t getWorkSize();
    };

    template <typename T, typename U>
    class FixedLengthModule: public Module<T, U> {
        public:
            bool canProcess() override;
            void process() override;
        protected:
            virtual void process(T* input, U* output) = 0;
            virtual size_t getLength() = 0;
    };
}