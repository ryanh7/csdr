#pragma once

#include "reader.hpp"

namespace Csdr {

    class UntypedSink {
        public:
            virtual ~UntypedSink() = default;
            virtual bool hasReader() = 0;
    };

    template <typename T>
    class Sink: public UntypedSink {
        public:
            virtual void setReader(Reader<T>* reader);
            virtual Reader<T>* getReader();
            bool hasReader() override;
        protected:
            Reader<T>* reader = nullptr;
    };


}