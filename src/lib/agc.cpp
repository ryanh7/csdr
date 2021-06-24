#include "agc.hpp"

#include <math.h>
#include <climits>

using namespace Csdr;

template <typename T>
void Agc<T>::process() {
    T reference = this->reference * max();
	float gain = last_gain;
	float last_peak = reference / last_gain; //approx.
	T input_abs;
	float error, dgain;

	float xk, vk, rk;
	float dt = 0.5;
	float beta = 0.005;

	size_t input_size = this->reader->available();
	T* input = this->reader->getReadPointer();
	T* output = (T*) malloc(sizeof(T) * input_size);

	for (int i = 0; i < input_size; i++) {
        //We skip samples containing 0, as the gain would be infinity for those to keep up with the reference.
		if (input[i] != 0) {
            //The error is the difference between the required gain at the actual sample, and the previous gain value.
            //We actually use an envelope detector.
            input_abs = this->abs(input[i]);
            error = (input_abs * gain) / reference;

			//An AGC is something nonlinear that's easier to implement in software:
			//if the amplitude decreases, we increase the gain by minimizing the gain error by attack_rate.
			//We also have a decay_rate that comes into consideration when the amplitude increases.
			//The higher these rates are, the faster is the response of the AGC to amplitude changes.
			//However, attack_rate should be higher than the decay_rate as we want to avoid clipping signals.
			//that had a sudden increase in their amplitude.
			//It's also important to note that this algorithm has an exponential gain ramp.

			if (error > 1) {
    			//INCREASE IN SIGNAL LEVEL
				if (last_peak < input_abs) {
					attack_wait_counter = attack_wait_time;
					last_peak = input_abs;
				}

				if (attack_wait_counter > 0) {
					attack_wait_counter--;
					dgain = 1;
				} else {
					//If the signal level increases, we decrease the gain quite fast.
					dgain = 1 - attack_rate;
					//Before starting to increase the gain next time, we will be waiting until hang_time for sure.
					hang_counter = hang_time;
				}
			} else {
			    //DECREASE IN SIGNAL LEVEL
				if (hang_counter > 0) {
					//Before starting to increase the gain, we will be waiting until hang_time.
					hang_counter--;
					dgain = 1; //..until then, AGC is inactive and gain doesn't change.
				} else {
				    dgain = 1 + decay_rate; //If the signal level decreases, we increase the gain quite slowly.
				}
			}
			gain *= dgain;
		}

        // alpha beta filter
		xk = this->xk + (this->vk * dt);
		vk = this->vk;

		rk = gain - xk;

		xk += gain_filter_alpha * rk;
		vk += (beta * rk) / dt;

		this->xk = xk;
		this->vk = vk;

		gain = this->xk;

        // clamp gain to max_gain and 0
		if (gain > max_gain) gain = max_gain;
		if (gain < 0) gain = 0;

        // actual sample scaling
        // limiting
        if (gain * input[i] > max()) {
            output[i] = max();
        } else if (gain * input[i] < min()) {
            output[i] = min();
        } else {
    		output[i] = gain * input[i];
        }
	}

	this->writer->write(output, input_size);
	this->reader->advance(input_size);

	free(output);

    last_gain=gain;
}

template <>
short Agc<short>::max() {
    return SHRT_MAX;
}

template <>
short Agc<short>::min() {
    return SHRT_MIN;
}

template <>
short Agc<short>::abs(short in) {
    return ::abs(in);
}

template <>
float Agc<float>::max() {
    return 1.0f;
}

template <>
float Agc<float>::min() {
    return -1.0f;
}

template <>
float Agc<float>::abs(float in) {
    return ::fabs(in);
}

template <typename T>
void Agc<T>::setReference(float reference) {
    this->reference = reference;
}

template <typename T>
void Agc<T>::setAttack(float attack_rate) {
    this->attack_rate = attack_rate;
}

template <typename T>
void Agc<T>::setDecay(float decay_rate) {
    this->decay_rate = decay_rate;
}

template <typename T>
void Agc<T>::setMaxGain(float max_gain) {
    this->max_gain = max_gain;
}

template <typename T>
void Agc<T>::setHangTime(unsigned long int hang_time) {
    this->hang_time = hang_time;
}

template class Agc<short>;
template class Agc<float>;