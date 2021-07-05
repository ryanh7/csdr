#include "commands.hpp"
#include "agc.hpp"
#include "fmdemod.hpp"
#include "amdemod.hpp"
#include "dcblock.hpp"
#include "converter.hpp"
#include "fft.hpp"
#include "logpower.hpp"
#include "logaveragepower.hpp"
#include "fftexchangesides.hpp"
#include "realpart.hpp"
#include "firdecimate.hpp"
#include "benchmark.hpp"
#include "fractionaldecimator.hpp"
#include "adpcm.hpp"

#include <iostream>
#include <cerrno>
#include <cstring>
#include <fcntl.h>

using namespace Csdr;

template <typename T, typename U>
void Command::runModule(Module<T, U>* module) {
    Ringbuffer<T>* buffer = new Ringbuffer<T>(bufferSize());
    module->setReader(new RingbufferReader<T>(buffer));
    module->setWriter(new StdoutWriter<U>());

    fd_set read_fds;
    struct timeval tv = { .tv_sec = 10, .tv_usec = 0};
    int rc;
    size_t read;
    size_t read_over = 0;
    int nfds = fileno(stdin) + 1;

    FILE* fifo = nullptr;
    char* fifo_input = nullptr;
    if (!fifoName.empty()) {
        fifo = fopen(fifoName.c_str(), "r");
        if (fifo == nullptr) {
            std::cerr << "error opening fifo: " << strerror(errno) << "\n";
        } else {
            fcntl(fileno(fifo), F_SETFL, O_NONBLOCK);
            nfds = std::max(fileno(stdin), fileno(fifo)) + 1;
            fifo_input = (char*) malloc(1024);
        }
    }

    bool run = true;
    while (run) {
        FD_ZERO(&read_fds);
        FD_SET(fileno(stdin), &read_fds);
        if (fifo) FD_SET(fileno(fifo), &read_fds);
        tv.tv_sec = 10;
        tv.tv_usec = 0;

        rc = select(nfds, &read_fds, NULL, NULL, &tv);
        if (rc == -1) {
            std::cerr << "select() error: " << strerror(errno) << "\n";
            run = false;
        } else if (rc) {
            if (FD_ISSET(fileno(stdin), &read_fds)) {
                // clamp so we don't overwrite the whole buffer in one go
                size_t writeable = std::min((size_t) 1024, buffer->writeable());
                writeable = (writeable * sizeof(T)) - read_over;
                read = std::cin.readsome(((char*) buffer->getWritePointer()) + read_over, writeable);
                if (read == 0) {
                    run = false;
                    break;
                }
                buffer->advance((read + read_over) / sizeof(T));
                read_over = (read + read_over) % sizeof(T);
                while (module->canProcess()) module->process();
            }
            if (fifo && FD_ISSET(fileno(fifo), &read_fds)) {
                if (fgets(fifo_input, 1024, fifo) != NULL) {
                    processFifoData(std::string(fifo_input, strlen(fifo_input) - 1));
                }
            }
        //} else {
            // no data, just timeout.
        }

        if (std::cin.eof()) {
            run = false;
        }

        if (fifo && feof(fifo)) {
            std::cerr << "WARNING: fifo indicates EOF, terminating\n";
            run = false;
        }

    }

    if (fifo) {
        fclose(fifo);
        free(fifo_input);
    }
    delete buffer;
    delete module;
}

void Command::addFifoOption() {
    add_option("--fifo", fifoName, "Control fifo");
}

AgcCommand::AgcCommand(): Command("agc", "Automatic gain control") {
    add_set("-f,--format", format, {"s16", "float"}, "Data format", true);
    add_set("-p,--profile", profile, {"fast", "slow"}, "AGC profile", true);
    add_option("-a,--attack", attack, "AGC attack rate (slow: 0.1; fast: 0.01)");
    add_option("-d,--decay", decay, "AGC decay rate (slow: 0.0001; fast: 0.001)");
    add_option("-t,--hangtime", hangtime, "AGC hang time (slow: 600; fast: 200)");
    add_option("-m,--max", max_gain, "Maximum gain", true);
    add_option("-i,--initial", initial_gain, "Initial gain", true);
    add_option("-r,--reference", reference, "Reference level", true);
    callback( [this] () {
        if (format == "float") {
            runAgc<float>();
        } else if (format == "s16") {
            runAgc<short>();
        } else {
            std::cerr << "invalid format: " << format << "\n";
        }
    });
}

template <typename T>
void AgcCommand::runAgc() {
    Agc<T>* agc = new Agc<T>();
    if (profile == "fast") {
        agc->setAttack(0.1);
        agc->setDecay(0.001);
        agc->setHangTime(200);
    } else if (profile == "slow") {
        agc->setAttack(0.01);
        agc->setDecay(0.0001);
        agc->setHangTime(600);
    }
    // override profile settings with explicit options on the command-line, if given
    if (attack != 0) agc->setAttack(attack);
    if (decay != 0) agc->setDecay(decay);
    if (hangtime != 0) agc->setHangTime(hangtime);
    agc->setMaxGain(max_gain);
    agc->setInitialGain(initial_gain);
    agc->setReference(reference);
    runModule(agc);
}

FmdemodCommand::FmdemodCommand(): Command("fmdemod", "FM demodulation") {
    callback( [this] () {
        runModule(new FmDemod());
    });
}

AmdemodCommand::AmdemodCommand(): Command("amdemod", "AM demodulation") {
    callback( [this] () {
        runModule(new AmDemod());
    });
}

DcBlockCommand::DcBlockCommand(): Command("dcblock", "DC block") {
    callback( [this] () {
        runModule(new DcBlock());
    });
}

ConvertCommand::ConvertCommand(): Command("convert", "Convert between stream formats") {
    add_set("-i,--informat", inFormat, {"s16", "float"}, "Input data format", true);
    add_set("-o,--outformat", outFormat, {"s16", "float"}, "Output data format", true);
    callback( [this] () {
        if (inFormat == outFormat) {
            std::cerr << "input and output format are identical, cannot convert\n";
            return;
        }
        if (inFormat == "s16") {
            if (outFormat == "float") {
                runModule(new Converter<short, float>());
            } else {
                std::cerr << "unable to handle output format \"" << outFormat << "\"\n";
            }
        } else if (inFormat == "float") {
            if (outFormat == "s16") {
                runModule(new Converter<float, short>());
            } else {
                std::cerr << "unable to handle output format \"" << outFormat << "\"\n";
            }
        } else {
            std::cerr << "unable to handle input format \"" << inFormat << "\"\n";
        }
    });
}

FftCommand::FftCommand(): Command("fft", "Fast Fourier transformation") {
    add_option("fft_size", fftSize, "FFT size")->required();
    add_option("every_n_samples", everyNSamples, "Run FFT every N samples")->required();
    add_set("-w,--window", window, {"boxcar", "blackman", "hamming"}, "Window function", true);
    callback( [this] () {
        if (!isPowerOf2(fftSize)) {
            std::cerr << "FFT size must be power of 2\n";
            return;
        }
        Window* w;
        if (window == "boxcar") {
            w = new BoxcarWindow();
        } else if (window == "blackman") {
            w = new BlackmanWindow();
        } else if (window == "hamming") {
            w = new HammingWindow();
        } else {
            std::cerr << "window type \"" << window << "\" not available\n";
            return;
        }

        runModule(new Fft(fftSize, everyNSamples, w));
    });
}

bool FftCommand::isPowerOf2(unsigned int size) {
    unsigned char bitcount = 0;
    for (int i = 0; i < 32; i++) {
        bitcount += (size >> i) & 1;
    }
    return bitcount == 1;
}

LogPowerCommand::LogPowerCommand(): Command("logpower", "Calculate dB power") {
    add_option("add_db", add_db, "Offset in dB", true);
    callback( [this] () {
        runModule(new LogPower(add_db));
    });
}

LogAveragePowerCommand::LogAveragePowerCommand(): Command("logaveragepower", "Calculate average dB power") {
    add_option("fft_size", fftSize, "Number of FFT bins")->required();
    add_option("avg_number", avgNumber, "Number of FFTs to average")->required();
    add_option("-a,--add", add_db, "Offset in dB", true);
    callback( [this] () {
        runModule(new LogAveragePower(fftSize, avgNumber, add_db));
    });
}

FftExchangeSidesCommand::FftExchangeSidesCommand(): Command("fftswap", "Switch FFT sides") {
    add_option("fft_size", fftSize, "Number of FFT bins")->required();
    callback( [this] () {
        runModule(new FftExchangeSides(fftSize));
    });
}

RealpartCommand::RealpartCommand(): Command("realpart", "Extract the real part of an IQ signal") {
    callback( [this] () {
        runModule(new Realpart());
    });
}

ShiftCommand::ShiftCommand(): Command("shift", "Shift a signal in the frequency domain") {
    add_option("rate", rate, "Amount of shift relative to the sampling rate");
    addFifoOption();
    callback( [this] () {
        //auto shift = new ShiftMath(rate);
        auto shift = new ShiftAddfast(rate);
        shiftModule = shift;
        runModule(shift);
    });
}

void ShiftCommand::processFifoData(std::string data) {
    shiftModule->setRate(std::stof(data));
}

FirDecimateCommand::FirDecimateCommand(): Command("firdecimate", "Decimate and filter") {
    add_option("decimation_factor", decimationFactor, "Decimation factor")->required();
    add_option("transition_bw", transitionBandwidth, "Transition bandwidth", true);
    add_set("-w,--window", window, {"boxcar", "blackman", "hamming"}, "Window function", true);
    callback( [this] () {
        Window* w;
        if (window == "boxcar") {
            w = new BoxcarWindow();
        } else if (window == "blackman") {
            w = new BlackmanWindow();
        } else if (window == "hamming") {
            w = new HammingWindow();
        } else {
            std::cerr << "window type \"" << window << "\" not available\n";
            return;
        }
        runModule(new FirDecimate(decimationFactor, transitionBandwidth, w));
    });
}

BenchmarkCommand::BenchmarkCommand(): Command("benchmark", "Perform internal benchmarks") {
    callback( [this] () {
        (new Benchmark())->run();
    });
}

FractionalDecimatorCommand::FractionalDecimatorCommand(): Command("fractionaldecimator", "Decimate in fractions") {
    add_set("-f,--format", format, {"float", "complex"}, "Format", true);
    add_option("decimation_rate", decimation_rate, "Decimation rate")->required();
    add_option("-n,--numpoly", num_poly_points, "Number of poly points", true);
    add_option("-t,--transition", transition, "Transition bandwidth for the prefilter", true);
    add_set("-w,--window", window, {"boxcar", "blackman", "hamming"}, "Window function for the prefilter", true);
    add_flag("-p,--prefilter", prefilter, "Apply filtering before decimation");
    callback( [this] () {
        if (format == "float") {
            runDecimator<float>();
        } else if (format == "complex") {
            runDecimator<complex<float>>();
        } else {
            std::cerr << "invalid format \"" << format << "\"\n";
        }
    });
}

template <typename T>
void FractionalDecimatorCommand::runDecimator() {
    FirFilter<T>* filter = nullptr;
    if (prefilter) {
        Window* w;
        if (window == "boxcar") {
            w = new BoxcarWindow();
        } else if (window == "blackman") {
            w = new BlackmanWindow();
        } else if (window == "hamming") {
            w = new HammingWindow();
        } else {
            std::cerr << "window type \"" << window << "\" not available\n";
            return;
        }
        filter = new LowPassFilter<T>(0.5 / (decimation_rate - transition), transition, w);
    }
    runModule(new FractionalDecimator<T>(decimation_rate, num_poly_points, filter));
}

AdpcmCommand::AdpcmCommand(): Command("adpcm", "ADPCM codec") {
    auto decodeFlag = add_flag("-d,--decode", decode, "Decode ADPCM data");
    auto encodeFlag = add_flag("-e,--encode", encode, "Encode into ADPCM data");
    // mutually exclusive
    encodeFlag->excludes(decodeFlag);
    decodeFlag->excludes(encodeFlag);
    callback( [this] () {
        // default is encode
        if (!decode) {
            runModule(new AdpcmEncoder());
        } else {
            runModule(new AdpcmDecoder());
        }
    });
}