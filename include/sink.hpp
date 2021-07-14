#pragma once

#include "reader.hpp"

namespace Csdr {

    template <typename T>
    class Sink {
        public:
            virtual ~Sink() = default;
            virtual void setReader(Reader<T>* reader);
            virtual Reader<T>* getReader();
        protected:
            Reader<T>* reader;
    };


}