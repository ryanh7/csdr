#include "module.hpp"

using namespace Csdr;

template <typename T, typename U>
Module<T, U>::Module(RingbufferReader<T>* in, Writer<U>* out) {
    reader = in;
    writer = out;
}

template class Module<short, short>;
template class Module<float, float>;