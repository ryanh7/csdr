#pragma once

#include <cstddef>
#include <ctime>

namespace Csdr {

    class Benchmark {
        public:
            void run();
            double timeTaken(struct ::timespec start, struct ::timespec end);
    };

}