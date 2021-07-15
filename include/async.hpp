#pragma once

#include "module.hpp"
#include <thread>

namespace Csdr {

    template <typename T, typename U>
    class AsyncRunner{
        public:
            explicit AsyncRunner(Module<T, U>* module);
            ~AsyncRunner();
            void stop();
        private:
            void loop();
            bool run = true;
            Module<T, U>*  module;
            std::thread thread;
    };

}