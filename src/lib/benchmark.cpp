#include "benchmark.hpp"
#include "complex.hpp"
#include "firdecimate.hpp"
#include "window.hpp"

#include <iostream>
#include <fcntl.h>
#include <unistd.h>

#define T_BUFSIZE (1024 * 1024 / 4)
#define T_N 200
#define T_DECFACT 10

using namespace Csdr;

void Benchmark::run() {
    std::cerr << "Getting " << T_BUFSIZE << " of random samples...\n";
    int urand_fp = open("/dev/urandom", O_RDWR);
    auto buf_u8 = (unsigned char*) malloc(sizeof(unsigned char) * T_BUFSIZE * 2);
    auto buf_c = (complex<float>*) malloc(sizeof(complex<float>) * T_BUFSIZE);
    read(urand_fp, buf_u8, T_BUFSIZE);
    close(urand_fp);

    for (int i = 0; i < T_BUFSIZE; i++) {
        buf_c[i].i(buf_u8[2 * i] / 128.0);
        buf_c[i].q(buf_u8[2 * i + 1] / 128.0);
    }

    auto fd = new FirDecimate(T_DECFACT, 0.00391389432485, new HammingWindow());
    auto reader = new MemoryReader<complex<float>>(buf_c, T_BUFSIZE);
    fd->setReader(reader);
    fd->setWriter(new VoidWriter<complex<float>>(T_BUFSIZE));

    struct ::timespec start_time, end_time;

    std::cerr << "Starting tests of processing " << T_BUFSIZE * T_N << " samples...\n";

    //fir_decimate_cc
    clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);
    for (int i = 0; i < T_N; i++) {
        while (fd->canProcess()) fd->process();
        reader->rewind();
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
    std::cerr << "firdecimate done in " << timeTaken(start_time, end_time) << " seconds.\n";
}

double Benchmark::timeTaken(struct ::timespec start, struct ::timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec-start.tv_nsec) / 1e9;
}