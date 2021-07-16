#include "async.hpp"
#include "complex.hpp"
#include "ringbuffer.hpp"

using namespace Csdr;

template<typename T, typename U>
AsyncRunner<T, U>::AsyncRunner(Module<T, U> *module):
    module(module),
    thread([this] { loop(); })
{}

template<typename T, typename U>
AsyncRunner<T, U>::~AsyncRunner() {
    stop();
    thread.join();
    delete module;
}

template<typename T, typename U>
void AsyncRunner<T, U>::stop() {
    run = false;
    if (module->hasReader()) {
        module->getReader()->unblock();
    }
}

template<typename T, typename U>
void AsyncRunner<T, U>::loop() {
    while (run) {
        while (module->canProcess()) {
            module->process();
        }
        try {
            module->getReader()->wait();
        } catch (const BufferError &) {
            break;
        }
    }
}

namespace Csdr {
    template class AsyncRunner<complex<float>, float>;
    template class AsyncRunner<float, float>;
    template class AsyncRunner<float, short>;
    template class AsyncRunner<short, float>;
    template class AsyncRunner<complex<float>, complex<float>>;
    template class AsyncRunner<short, unsigned char>;
    template class AsyncRunner<unsigned char, short>;
    template class AsyncRunner<float, unsigned char>;
    template class AsyncRunner<complex<float>, unsigned char>;
    template class AsyncRunner<unsigned char, unsigned char>;
    template class AsyncRunner<short, short>;
}