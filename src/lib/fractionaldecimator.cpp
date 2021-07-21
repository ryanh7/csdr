#include "fractionaldecimator.hpp"

using namespace Csdr;

template <typename T>
FractionalDecimator<T>::FractionalDecimator(float rate, unsigned int num_poly_points, FirFilter<T, float> *filter):
    num_poly_points(num_poly_points &~ 1),
    poly_precalc_denomiator((float*) malloc(this->num_poly_points * sizeof(float))),
    xifirst(-(this->num_poly_points / 2) + 1),
    xilast(this->num_poly_points / 2),
    coeffs_buf((float*) malloc(this->num_poly_points * sizeof(float))),
    rate(rate),
    filter(filter)
{
    int id = 0; //index in poly_precalc_denomiator
    for (int xi = xifirst; xi <= xilast; xi++) {
        poly_precalc_denomiator[id] = 1;
        for(int xj = xifirst; xj <= xilast; xj++) {
            //poly_precalc_denomiator could be integer as well. But that would later add a necessary conversion.
            if (xi != xj) poly_precalc_denomiator[id] *= (xi - xj);
        }
        id++;
    }

    where = -xifirst;
}

template <typename T>
FractionalDecimator<T>::~FractionalDecimator() {
    free(poly_precalc_denomiator);
    free(coeffs_buf);
}

template <typename T>
bool FractionalDecimator<T>::canProcess() {
    size_t size = std::min(this->reader->available(), (size_t) ceilf(this->writer->writeable() / rate));
    size_t filterLen = filter != nullptr ? filter->getOverhead() : 0;
    return ceilf(where) + num_poly_points + filterLen < size;
}

template <typename T>
void FractionalDecimator<T>::process() {
    //This routine can handle floating point decimation rates.
    //It applies polynomial interpolation to samples that are taken into consideration from a pre-filtered input.
    //The pre-filter can be switched off by applying filter = nullptr.
    int oi = 0; //output index
    int index_high, index;
    size_t size = std::min(this->reader->available(), (size_t) ceilf(this->writer->writeable() / rate));
    size_t filterLen = filter != nullptr ? filter->getOverhead() : 0;
    T* input = this->reader->getReadPointer();
    T* output = this->writer->getWritePointer();
    //we optimize to calculate ceilf(where) only once every iteration, so we do it here:
    while ((index_high = ceilf(where)) + num_poly_points + filterLen < size) {
        // num_poly_points above is theoretically more than we could have here, but this makes the spectrum look good
        index = index_high - 1;
        int id = 0;
        float xwhere = where - index;
        for (int xi = xifirst; xi <= xilast; xi++) {
            coeffs_buf[id] = 1;
            for (int xj = xifirst; xj <= xilast; xj++) {
                if (xi != xj) coeffs_buf[id] *= (xwhere - xj);
            }
            id++;
        }
        T acc = 0;
        if (filter != nullptr) {
            for (int i = 0; i < num_poly_points; i++) {
                SparseView<T> sparse = filter->sparse(input);
                acc += (coeffs_buf[i] / poly_precalc_denomiator[i]) * sparse[index + i];
            }
        } else {
            for (int i = 0; i < num_poly_points; i++) {
                acc += (coeffs_buf[i] / poly_precalc_denomiator[i]) * input[index + i];
            }
        }
        output[oi++] = acc;
        where += rate;
    }

    int input_processed = index + xifirst;
    where -= input_processed;

    this->reader->advance(input_processed);
    this->writer->advance(oi);
}

namespace Csdr {
    template class FractionalDecimator<float>;
    template class FractionalDecimator<complex<float>>;
}