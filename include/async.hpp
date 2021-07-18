#pragma once

#include "module.hpp"
#include <thread>

namespace Csdr {

    class UntypedRunner {
        public:
            virtual ~UntypedRunner() = default;
            virtual void stop() = 0;
    };

    template <typename T, typename U>
    class AsyncRunner: public UntypedRunner {
        public:
            explicit AsyncRunner(Module<T, U>* module);
            ~AsyncRunner() override;
            void stop() override;
        private:
            void loop();
            bool run = true;
            Module<T, U>*  module;
            std::thread thread;
    };

}