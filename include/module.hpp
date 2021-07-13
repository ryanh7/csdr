#pragma once

#include "reader.hpp"
#include "writer.hpp"
#include "complex.hpp"

#include <cstdint>

namespace Csdr {

    template <typename T, typename U>
    class Module: public Sink<T>, public Source<U> {
        public:
            virtual ~Module() = default;
            virtual bool canProcess() = 0;
            virtual void process() = 0;
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