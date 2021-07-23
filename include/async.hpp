#pragma once

#include "module.hpp"
#include <thread>

namespace Csdr {

    class UntypedRunner {
        public:
            virtual ~UntypedRunner() = default;
            virtual void stop() = 0;
    };

    class AsyncRunner: public UntypedRunner {
        public:
            explicit AsyncRunner(UntypedModule* module);
            ~AsyncRunner() override;
            void stop() override;
        private:
            void loop();
            bool run = true;
            UntypedModule*  module;
            std::thread thread;
    };

}