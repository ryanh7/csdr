#pragma once

#include "module.hpp"

namespace Csdr {

    class Cli {
        public:
            int main(int argc, char** argv);
        private:
            template <typename T, typename U>
            void runModule(Module<T, U>* module);
    };

}