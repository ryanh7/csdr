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
    run = false;
    module->unblock();
    if (thread.joinable()) thread.join();
}

void AsyncRunner::loop() {
    while (run) {
        while (module->canProcess()) {
            module->process();
        }
        try {
            module->wait();
        } catch (const BufferError &) {
            break;
        }
    }
}