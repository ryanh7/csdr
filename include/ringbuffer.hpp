#pragma once

#include "reader.hpp"
#include "writer.hpp"

#include <cstdlib>
#include <stdexcept>
#include <unistd.h>
#include <mutex>
#include <condition_variable>

namespace Csdr {

    class BufferError: public std::runtime_error {
        public:
            explicit BufferError(const std::string& err): std::runtime_error(err) {}
    };

    template <typename T>
    class Ringbuffer: public Writer<T> {
        public:
            explicit Ringbuffer<T>(size_t size);
            ~Ringbuffer() override;
            size_t writeable() override;
            T* getWritePointer() override;
            T* getPointer(size_t pos);
            void advance(size_t how_much) override;
            void advance(size_t& what, size_t how_much);
            size_t available(size_t read_pos);
            size_t getWritePos();
            void wait();
        private:
            T* allocate_mirrored(size_t size);
            T* data = nullptr;
            size_t size;
            size_t write_pos = 0;
            std::mutex mutex;
            std::condition_variable condition;
    };

    template <typename T>
    class RingbufferReader: public Reader<T> {
        public:
            explicit RingbufferReader<T>(Ringbuffer<T>* buffer);
            size_t available() override;
            T* getReadPointer() override;
            void advance(size_t how_much) override;
            void wait() override;
        private:
            Ringbuffer<T>* buffer;
            size_t read_pos;
    };

}