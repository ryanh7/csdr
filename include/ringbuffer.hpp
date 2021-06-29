#pragma once

#include <cstdlib>

namespace Csdr {

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