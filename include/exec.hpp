#pragma once

#include "module.hpp"

#include <vector>

namespace Csdr {

    template<typename T, typename U>
    class ExecModule: public Module<T, U> {
        public:
            explicit ExecModule(std::vector<std::string> args);
            ~ExecModule();
            bool canProcess() override;
            void process() override;
            void setWriter(Writer<U>* writer) override;
        private:
            void startChild();
            void readLoop();
            std::vector<std::string> args;
            pid_t child_pid = 0;
            int readPipe;
            int writePipe;
            std::thread* readThread = nullptr;
            bool run = true;
    };

}