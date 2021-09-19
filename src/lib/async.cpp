/*
Copyright (c) 2021 Jakob Ketterl <jakob.ketterl@gmx.de>

This file is part of libcsdr.

libcsdr is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "async.hpp"
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