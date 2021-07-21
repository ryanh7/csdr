#pragma once

#include <cstddef>
#include <ctime>

#include "module.hpp"

namespace Csdr {

    class Benchmark {
        public:
            void run();
            template <typename T, typename U>
            void runModule(Module<T, U>* module);
            template <typename T>
            T* getTestData();
            double timeTaken(struct ::timespec start, struct ::timespec end);
    };

}