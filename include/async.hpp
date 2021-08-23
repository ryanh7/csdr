#pragma once

#include "module.hpp"
#include <thread>

namespace Csdr {

    class AsyncRunner {
        public:
            explicit AsyncRunner(UntypedModule* module);
            ~AsyncRunner();
            void stop();
            bool isRunning() const;
        private:
            void loop();
            bool run = true;
            UntypedModule*  module;
            std::thread thread;
    };

}