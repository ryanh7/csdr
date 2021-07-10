#include "timingrecovery.hpp"

using namespace Csdr;

TimingRecovery::TimingRecovery(unsigned int decimation, float loop_gain, float max_error, bool use_q):
    decimation(decimation),
    loop_gain(loop_gain),
    max_error(max_error),
    use_q(use_q)
{}

bool TimingRecovery::canProcess() {
    return reader->available() > (decimation / 2) * 3 && writer->writeable() > 0;
}

void TimingRecovery::process() {
    //We always assume that the input starts at center of the first symbol cross before the first symbol.
    //Last time we consumed that much from the input samples that it is there.
    unsigned int num_samples_bit = decimation;
    unsigned int num_samples_halfbit = decimation / 2;
    unsigned int num_samples_quarterbit = decimation / 4;

    if (correction_offset <= 0.9 * -num_samples_quarterbit || correction_offset >= 0.9 * num_samples_quarterbit) {
        correction_offset = 0;
    }

    //should check if the sign of the correction_offset (or disabling it) has an effect on the EVM.
    //it is also a possibility to disable multiplying with the magnitude
    float error = getError();

    if (error > max_error) error = max_error;
    if (error < -max_error) error = -max_error;

    correction_offset = error * loop_gain * getErrorSign() * num_samples_halfbit;

    reader->advance(num_samples_bit + correction_offset);
}

float TimingRecovery::calculateError(int el_point_right_index, int el_point_left_index, int el_point_mid_index) {
    complex<float>* input = reader->getReadPointer();
    float error = input[el_point_right_index].i() - input[el_point_left_index].i() * input[el_point_mid_index].i();
    if (use_q) {
        error = input[el_point_right_index].q() - input[el_point_left_index].q() * input[el_point_mid_index].q();
        error /= 2;
    }
    return error;
}

float GardnerTimingRecovery::getError() {
    complex<float>* input = reader->getReadPointer();
    //maximum effect point is at current_bitstart_index
    int num_samples_halfbit = decimation / 2;

    int el_point_right_index  = num_samples_halfbit * 3;
    int el_point_left_index   = num_samples_halfbit * 1;
    int el_point_mid_index    = num_samples_halfbit * 2;
    *(writer->getWritePointer()) = input[el_point_left_index];
    writer->advance(1);

    return calculateError(el_point_right_index, el_point_left_index, el_point_mid_index);
}

float EarlyLateTimingRecovery::getError() {
    complex<float>* input = reader->getReadPointer();
    int num_samples_bit = decimation;
    int num_samples_halfbit = decimation / 2;
    int num_samples_earlylate_wing = num_samples_bit * earlylate_ratio;

    int el_point_right_index  = num_samples_earlylate_wing * 3;
    int el_point_left_index   = num_samples_earlylate_wing - correction_offset;
    int el_point_mid_index    = num_samples_halfbit;
    *(writer->getWritePointer()) = input[el_point_mid_index];
    writer->advance(1);

    return calculateError(el_point_right_index, el_point_left_index, el_point_mid_index);
}