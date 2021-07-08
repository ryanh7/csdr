#pragma once

#include "module.hpp"
#include "complex.hpp"
#include "fir.hpp"

namespace Csdr {

    template <typename T>
    class FractionalDecimator: public Module<T, T> {
        public:
            FractionalDecimator(float rate, unsigned int num_poly_points, FirFilter<T, float>* filter = nullptr);
            bool canProcess() override;
            void process() override;
        private:
            float where;
            unsigned int num_poly_points; //number of samples that the Lagrange interpolator will use
            float* poly_precalc_denomiator; //while we don't precalculate coefficients here as in a Farrow structure, because it is a fractional interpolator, but we rather precaculate part of the interpolator expression
            //float* last_inputs_circbuf; //circular buffer to store the last (num_poly_points) number of input samples.
            //int last_inputs_startsat; //where the circular buffer starts now
            //int last_inputs_samplewhere;
            float* coeffs_buf;
            int xifirst;
            int xilast;
            float rate;
            FirFilter<T, float>* filter;
    };

}