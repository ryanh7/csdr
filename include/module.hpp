#pragma once

#include "ringbuffer.hpp"
#include "writer.hpp"

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

}