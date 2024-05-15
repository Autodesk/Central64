#pragma once
#ifndef CENTRAL64_ARRAY2D
#define CENTRAL64_ARRAY2D

#include <central64/grid/Offset2D.hpp>

namespace central64 {

/// A 2D array data structure for elements of type `T`.
template <typename T>
class Array2D
{
public:
    using Reference = typename std::vector<T>::reference;             ///< The reference type for an individual array element.
    using ConstReference = typename std::vector<T>::const_reference;  ///< The const reference type for an individual array element.

    Array2D() {}                             ///< Create a 2D array with dimensions [0, 0].
    Array2D(Offset2D dims);                  ///< Create a 2D array with dimensions `dims`.
    Array2D(Offset2D dims, const T& value);  ///< Create a 2D array with dimensions `dims` and all elements set to `value`.

    void Fill(const T& value);  ///< Set every element to `value`.

    Offset2D Dims() const { return dims_; }           ///< Get the dimensions.
    int Size() const { return dims_.X()*dims_.Y(); }  ///< Get the total number of elements.

    bool Contains(Offset2D coords) const;  ///< Check whether coordinates `coords` are within the array.

    Reference operator[](Offset2D coords);             ///< Obtain a reference to the element at coordinates `coords`.
    ConstReference operator[](Offset2D coords) const;  ///< Obtain a const reference to the element at coordinates `coords`.

private:
    Offset2D dims_{ 0, 0 };
    std::vector<T> data_{};
};

template <typename T>
Array2D<T>::Array2D(Offset2D dims)
{
    assert(dims.X() >= 0);
    assert(dims.Y() >= 0);

    dims_ = dims;
    data_.resize(dims_.X()*dims_.Y());
}

template <typename T>
Array2D<T>::Array2D(Offset2D dims, const T& value)
{
    assert(dims.X() >= 0);
    assert(dims.Y() >= 0);

    dims_ = dims;
    data_.assign(dims_.X()*dims_.Y(), value);
}

template <typename T>
void Array2D<T>::Fill(const T& value)
{
    data_.assign(dims_.X()*dims_.Y(), value);
}

template <typename T>
inline bool Array2D<T>::Contains(Offset2D coords) const
{
    return (coords.X() >= 0) &&
           (coords.Y() >= 0) &&
           (coords.X() < dims_.X()) &&
           (coords.Y() < dims_.Y());
}

template <typename T>
inline typename Array2D<T>::Reference Array2D<T>::operator[](Offset2D coords)
{
    assert(Contains(coords));

    return data_[coords.Y()*dims_.X() + coords.X()];
}

template <typename T>
inline typename Array2D<T>::ConstReference Array2D<T>::operator[](Offset2D coords) const
{
    assert(Contains(coords));

    return data_[coords.Y()*dims_.X() + coords.X()];
}

}  // namespace

#endif
