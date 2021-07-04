#include "csdr++.hpp"
#include "ringbuffer.hpp"
#include "writer.hpp"
#include "agc.hpp"
#include "commands.hpp"

#include <iostream>

#include "CLI11.hpp"

using namespace Csdr;

int main (int argc, char** argv) {
    std::cin.sync_with_stdio(false);

    Cli server;
    return server.main(argc, argv);
}

int Cli::main(int argc, char** argv) {
    CLI::App app;

    CLI::Option* version_flag = app.add_flag("-v,--version", "Display version information");

    app.add_subcommand(std::shared_ptr<CLI::App>(new AgcCommand()));
    app.add_subcommand(std::shared_ptr<CLI::App>(new FmdemodCommand()));
    app.add_subcommand(std::shared_ptr<CLI::App>(new AmdemodCommand()));
    app.add_subcommand(std::shared_ptr<CLI::App>(new DcBlockCommand()));
    app.add_subcommand(std::shared_ptr<CLI::App>(new ConvertCommand()));
    app.add_subcommand(std::shared_ptr<CLI::App>(new FftCommand()));
    app.add_subcommand(std::shared_ptr<CLI::App>(new LogPowerCommand()));
    app.add_subcommand(std::shared_ptr<CLI::App>(new LogAveragePowerCommand()));
    app.add_subcommand(std::shared_ptr<CLI::App>(new FftExchangeSidesCommand()));
    app.add_subcommand(std::shared_ptr<CLI::App>(new RealpartCommand()));
    app.add_subcommand(std::shared_ptr<CLI::App>(new ShiftCommand()));
    app.add_subcommand(std::shared_ptr<CLI::App>(new FirDecimateCommand()));
    app.add_subcommand(std::shared_ptr<CLI::App>(new FractionalDecimatorCommand()));

    app.add_subcommand(std::shared_ptr<CLI::App>(new BenchmarkCommand()));

    app.require_subcommand(1);

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        if (*version_flag) {
            std::cerr << "csdr++ version " << VERSION << "\n";
            return 0;
        }

        return app.exit(e);
    }

    return 0;
}