#include "filter.hpp"
#include "complex.hpp"

using namespace Csdr;

template<typename T>
SparseView<T> SampleFilter<T>::sparse(T* data) {
    return SparseView<T>(data, this);
}

template <typename T>
size_t SampleFilter<T>::apply(T *input, T *output, size_t size) {
    for (size_t i = 0; i < size; i++) {
        output[i] = processSample(input, i);
    }
    return size;
}

template<typename T>
SparseView<T>::SparseView(T *data, SampleFilter<T> *filter):
    data(data),
    filter(filter)
{}

template<typename T>
T SparseView<T>::operator[](size_t index) {
    return filter->processSample(data, index);
}

template <typename T>
FilterModule<T>::FilterModule(Filter<T> *filter): filter(filter) {}

template <typename T>
FilterModule<T>::~FilterModule() {
    delete this->filter;
}

template <typename T>
void FilterModule<T>::setFilter(Filter<T>* filter) {
    delete this->filter;
    this->filter = filter;
}

template <typename T>
bool FilterModule<T>::canProcess() {
    return this->reader->available() > filter->getMinProcessingSize() + filter->getOverhead() && this->writer->writeable() > filter->getMinProcessingSize();
}

template <typename T>
void FilterModule<T>::process() {
    T* input = this->reader->getReadPointer();
    T* output = this->writer->getWritePointer();
    size_t size = std::min(this->reader->available() - filter->getOverhead(), this->writer->writeable());
    size = filter->apply(input, output, size);
    this->reader->advance(size);
    this->writer->advance(size);
}

namespace Csdr {
    template class SampleFilter<complex<float>>;
    template class SampleFilter<float>;

    template class SparseView<complex<float>>;
    template class SparseView<float>;

    template class FilterModule<complex<float>>;
    template class FilterModule<float>;
}