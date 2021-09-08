#pragma once

#include "module.hpp"

namespace Csdr {

    template <typename T>
    class Downmix: public Module<T, T> {
        public:
            explicit Downmix(unsigned int channels = 2);
            bool canProcess() override;
            void process() override;
        private:
            unsigned int channels;
    };

}