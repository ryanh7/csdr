#pragma once

#include "CLI11.hpp"
#include "module.hpp"

namespace Csdr {

    class Command: public CLI::App {
        public:
            Command(std::string name, std::string description): CLI::App(description, name) {}
        protected:
            template <typename T, typename U>
            void runModule(Module<T, U>* module);
    };

    class AgcCommand: public Command {
        public:
            AgcCommand();
        private:
            template <typename T>
            void runAgc();
            std::string format = "float";
            std::string profile = "fast";
            unsigned long int hangtime = 0;
            float attack = 0;
            float decay = 0;
            float max_gain = 65535;
            float reference = 0.8;
            float initial_gain = 1;
    };

    class FmdemodCommand: public Command {
        public:
            FmdemodCommand();
    };

}