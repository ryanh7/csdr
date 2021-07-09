#include "varicode.hpp"

using namespace Csdr;

constexpr varicode_item VaricodeDecoder::varicode_items[];

bool VaricodeDecoder::canProcess() {
    return reader->available() > 0 && writer->writeable() > 0;
}

void VaricodeDecoder::process() {
    unsigned char symbol = *(reader->getReadPointer());
    reader->advance(1);

    status = (status << 1) | (symbol & 0b1); //shift new bit in shift register

    if ((status & 0xFFF) == 0) return;

    for (auto item: varicode_items) {
        unsigned long long mask = (1 << (item.bitcount + 4)) - 1;
        if ((item.code << 2) == (status & mask)) {
            *(writer->getWritePointer()) = item.ascii;
            writer->advance(1);
        }
    }
}
