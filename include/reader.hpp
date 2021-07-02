#pragma once

#include <cstdlib>

namespace Csdr {

    template <typename T>
    class Reader {
        public:
            virtual size_t available() = 0;
            virtual T* getReadPointer() = 0;
            virtual void advance(size_t how_much) = 0;
    };

}