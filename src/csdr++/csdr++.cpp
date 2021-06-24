#include "csdr++.hpp"
#include "ringbuffer.hpp"
#include "writer.hpp"
#include "agc.hpp"

#include <iostream>
#include <sys/select.h>
#include <algorithm>
#include <string.h>
#include <errno.h>

#include "CLI11.hpp"

using namespace Csdr;

int main (int argc, char** argv) {
    Cli server;
    return server.main(argc, argv);
}

int Cli::main(int argc, char** argv) {
    std::cin.sync_with_stdio(false);

    CLI::App app;

    CLI::Option* version_flag = app.add_flag("-v,--version", "Display version information");

    CLI::App* agc = app.add_subcommand("agc", "Automatic gain control");
    std::string agc_format = "float";
    agc->add_set("-f,--format", agc_format, {"s16", "float"}, "Data format (default: float)");
    float agc_max_gain = 65535;
    agc->add_option("-m,--max", agc_max_gain, "Maximum gain (default: 65535)");
    float agc_initial_gain = 1;
    agc->add_option("-i,--initial", agc_initial_gain, "Initial gain (default: 1)");
    agc->callback( [&] () {
        if (agc_format == "float") {
            Agc<float>* agc = new Agc<float>();
            agc->setMaxGain(agc_max_gain);
            agc->setInitialGain(agc_initial_gain);
            runModule(agc);
        } else if (agc_format == "s16") {
            Agc<short>* agc = new Agc<short>();
            agc->setMaxGain(agc_max_gain);
            agc->setInitialGain(agc_initial_gain);
            runModule(agc);
        }
    });

    app.require_subcommand(1);

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    if (*version_flag) {
        std::cerr << "csdr++ version " << VERSION << "\n";
        return 0;
    }

    return 0;
}

template <typename T, typename U>
void Cli::runModule(Module<T, U>* module) {
    Ringbuffer<T>* buffer = new Ringbuffer<T>(10240);
    module->setReader(new RingbufferReader<T>(buffer));
    module->setWriter(new StdoutWriter<U>());

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
                read = std::cin.readsome(((char*) buffer->getWritePointer()) + read_over, writeable * sizeof(T));
                if (read == 0) {
                    run = false;
                    break;
                }
                buffer->advance((read + read_over) / sizeof(T));
                read_over = (read + read_over) % sizeof(T);
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
}