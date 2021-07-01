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
#include "shift.hpp"

#include <iostream>
#include <errno.h>
#include <cstring>

using namespace Csdr;

template <typename T, typename U>
void Command::runModule(Module<T, U>* module) {
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
    add_option("rate", rate, "Amount of shift relative to the sampling rate")->required();
    callback( [this] () {
        runModule(new ShiftAddfast(rate));
    });
}