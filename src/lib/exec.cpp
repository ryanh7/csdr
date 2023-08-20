#include "exec.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <thread>
#include <utility>
#include <iostream>

using namespace Csdr;

template <typename T, typename U>
ExecModule<T, U>::ExecModule(std::vector<std::string> args):
    Module<T, U>(),
    args(std::move(args))
{
    startChild();
}

template <typename T, typename U>
ExecModule<T, U>::~ExecModule<T, U>() {
    stopChild();
}

template <typename T, typename U>
void ExecModule<T, U>::startChild() {
    size_t s = args.size();
    char* c_args[s];
    for (size_t i = 0; i < s; i++) {
        c_args[i] = (char*) args[i].c_str();
    }
    c_args[s] = NULL;

    int readPipes[2];
    pipe(readPipes);
    int writePipes[2];
    pipe(writePipes);

    child_pid = fork();
    int r;
    switch (child_pid) {
        case -1:
            throw std::runtime_error("could not fork");
        case 0:
            // we are the child.
            // set up pipes and exec()
            close(readPipes[0]);
            dup2(readPipes[1], STDOUT_FILENO);
            close(readPipes[1]);

            close(writePipes[1]);
            dup2(writePipes[0], STDIN_FILENO);
            close(writePipes[0]);

            r = execvp(c_args[0], c_args);
            if (r != 0) {
                throw std::runtime_error("could not exec");
            }
            break;
        default:
            // we are the parent; pid is the child's PID
            // set up pipes and reader thread
            close(readPipes[1]);
            this->readPipe = readPipes[0];
            close(writePipes[0]);
            this->writePipe = writePipes[1];
            if (this->writer != nullptr) {
                run = true;
                readThread = new std::thread([this] { readLoop(); });
            }
            break;
    }
}

template <typename T, typename U>
void ExecModule<T, U>::stopChild() {
    run = false;
    if (child_pid != 0) {
        kill(child_pid, SIGTERM);
        close(this->readPipe);
        close(this->writePipe);
        waitpid(child_pid, NULL, 0);
    }
    if (readThread != nullptr) {
        readThread->join();
        delete readThread;
        readThread = nullptr;
    }
}

template <typename T, typename U>
void ExecModule<T, U>::readLoop() {
    std::cerr << "read lopp starting\n";
    size_t available;
    size_t read_bytes;
    while (run) {
        available = std::min(this->writer->writeable(), (size_t) 1024) * sizeof(U) - offset;
        read_bytes = read(this->readPipe, ((char*) this->writer->getWritePointer()) + offset, available);
        if (read_bytes <= 0) {
            run = false;
        } else {
            this->writer->advance((offset + read_bytes) / sizeof(U));
            offset = (offset + read_bytes) % sizeof(U);
        }
    }
    std::cerr << "read loop ending\n";
}

template <typename T, typename U>
void ExecModule<T, U>::setWriter(Writer<U> *writer) {
    Module<T, U>::setWriter(writer);
    if (writer != nullptr && readThread == nullptr) {
        run = true;
        readThread = new std::thread([this] { readLoop(); });
    }
}

template <typename T, typename U>
bool ExecModule<T, U>::canProcess() {
    return this->writePipe != -1 && this->reader->available() > 0;
}

template <typename T, typename U>
void ExecModule<T, U>::process() {
    size_t size = std::min(this->reader->available(), (size_t) 1024);
    write(this->writePipe, this->reader->getReadPointer(), size * sizeof(T));
    this->reader->advance(size);
}

template <typename T, typename U>
void ExecModule<T, U>::reload() {
    if (this->child_pid != 0) {
        kill(this->child_pid, SIGHUP);
    }
}

template <typename T, typename U>
void ExecModule<T, U>::restart() {
    stopChild();
    startChild();
}

namespace Csdr {
    template class ExecModule<complex<short>, short>;
    template class ExecModule<short, short>;
    template class ExecModule<short, unsigned char>;
}