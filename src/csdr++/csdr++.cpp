#include "csdr++.hpp"
#include "ringbuffer.hpp"
#include "writer.hpp"
#include "module.hpp"
#include "agc.hpp"

#include <iostream>
#include <sys/select.h>
#include <algorithm>
#include <string.h>
#include <errno.h>

using namespace Csdr;

int main (int argc, char** argv) {
    Cli server;
    return server.main(argc, argv);
}

int Cli::main(int argc, char** argv) {
    std::cin.sync_with_stdio(false);

    Ringbuffer<short>* buffer = new Ringbuffer<short>(10240);
    RingbufferReader<short>* reader = new RingbufferReader<short>(buffer);
    Writer<short>* writer = new StdoutWriter<short>();
    Module<short, short>* module = new Agc<short>(reader, writer);

    fd_set read_fds;
    struct timeval tv;
    int rc;
    int nfds = fileno(stdin) + 1;
    size_t read;
    size_t read_over = 0;

    bool run = true;
    while (run) {
        FD_ZERO(&read_fds);
        FD_SET(fileno(stdin), &read_fds);
        tv.tv_sec = 10;
        tv.tv_usec = 0;

        rc = select(nfds, &read_fds, NULL, NULL, &tv);
        if (rc == -1) {
            std::cerr << "select() error: " << strerror(errno) << "\n";
            run = false;
        } else if (rc) {
            if (FD_ISSET(fileno(stdin), &read_fds)) {
                int writeable = buffer->writeable();
                // clamp so we don't woverwrite the whole buffer in one go
                if (writeable > 1024) writeable = 1024;
                read = std::cin.readsome(((char*) buffer->getWritePointer()) + read_over, writeable * sizeof(short));
                if (read == 0) {
                    run = false;
                    break;
                }
                buffer->advance((read + read_over) / sizeof(short));
                read_over = (read + read_over) % sizeof(short);
                module->process();
            }
        //} else {
            // no data, just timeout.
        }

        if (std::cin.eof()) {
            run = false;
        }

    }

    delete buffer;
    delete module;
    return 0;
}