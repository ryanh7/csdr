#pragma once

#include <cstdlib>
#include <stdexcept>
#include <unistd.h>

namespace Csdr {

    class BufferError: public std::runtime_error {
        public:
            BufferError(std::string err): std::runtime_error(err) {}
    };

    template <typename T>
    class Ringbuffer {
        public:
            Ringbuffer<T>(size_t size);
            ~Ringbuffer();
            size_t writeable();
            T* getWritePointer();
            T* getPointer(size_t pos);
            void advance(size_t how_much);
            void advance(size_t& what, size_t how_much);
            size_t available(size_t read_pos);
            size_t getWritePos();
        private:
            T* allocate_mirrored(size_t size);
            T* data;
            size_t size;
            size_t write_pos = 0;
    };

    template <typename T>
    class RingbufferReader {
        public:
            RingbufferReader<T>(Ringbuffer<T>* buffer);
            size_t available();
            T* getReadPointer();
            void advance(size_t how_much);
        private:
            Ringbuffer<T>* buffer;
            size_t read_pos;
    };

}