#include "async.hpp"
#include "complex.hpp"
#include "ringbuffer.hpp"

using namespace Csdr;

AsyncRunner::AsyncRunner(UntypedModule* module):
    module(module),
    thread([this] { loop(); })
{}

AsyncRunner::~AsyncRunner() {
    stop();
}

void AsyncRunner::stop() {
    if (run) {
        run = false;
        module->unblock();
    }
    if (thread.joinable()) thread.join();
}

bool AsyncRunner::isRunning() const {
    return run;
}

void AsyncRunner::loop() {
    while (run) {
        try {
            if (module->canProcess()) {
                module->process();
            } else {
                module->wait();
            }
        } catch (const BufferError&) {
            run = false;
            break;
        }
    }
}