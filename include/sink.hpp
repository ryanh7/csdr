#pragma once

#include "reader.hpp"

namespace Csdr {

    template <typename T>
    class Sink {
        public:
            virtual ~Sink() = default;
            virtual void setReader(Reader<T>* reader);
            virtual Reader<T>* getReader();
            virtual bool hasReader();
        protected:
            Reader<T>* reader = nullptr;
    };


}