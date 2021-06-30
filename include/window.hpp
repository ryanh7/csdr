#pragma once

#include <cstdlib>

namespace Csdr {

    class PrecalculatedWindow {
        public:
            PrecalculatedWindow(float* windowt, size_t size);
            ~PrecalculatedWindow();
            template <typename T>
            void apply(T* input, T* output, size_t size);
        private:
            float* windowt;
            size_t size;
    };

    class Window {
        public:
            template <typename T>
            void apply(T* input, T* output, size_t size);
            PrecalculatedWindow* precalculate(size_t size);
        protected:
            virtual float kernel(float rate) = 0;
    };

    class BoxcarWindow: public Window {
        protected:
            float kernel(float rate) override;
    };

    class BlackmanWindow: public Window {
        protected:
            float kernel(float rate) override;
    };

    class HammingWindow: public Window {
        protected:
            float kernel(float rate) override;
    };

}