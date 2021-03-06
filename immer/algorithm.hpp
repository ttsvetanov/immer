//
// immer - immutable data structures for C++
// Copyright (C) 2016, 2017 Juan Pedro Bolivar Puente
//
// This file is part of immer.
//
// immer is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// immer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with immer.  If not, see <http://www.gnu.org/licenses/>.
//

#pragma once

#include <algorithm>
#include <numeric>
#include <type_traits>

namespace immer {

/**
 * @defgroup algorithm
 * @{
 */

/*@{*/
// Right now these algorithms dispatch directly to the vector
// implementations unconditionally.  This will be changed in the
// future to support other kinds of containers.

/*!
 * Apply operation `fn` for every contiguous *chunk* of data in the
 * range sequentially.  Each time, `Fn` is passed two `value_type`
 * pointers describing a range over a part of the vector.  This allows
 * iterating over the elements in the most efficient way.
 *
 * @rst
 *
 * .. tip:: This is a low level method. Most of the time, :doc:`other
 *    wrapper algorithms <algorithms>` should be used instead.
 *
 * @endrst
 */
template <typename Range, typename Fn>
void for_each_chunk(const Range& r, Fn&& fn)
{
    r.impl().for_each_chunk(std::forward<Fn>(fn));
}

template <typename Iterator, typename Fn>
void for_each_chunk(const Iterator& first, const Iterator& last, Fn&& fn)
{
    assert(&first.impl() == &last.impl());
    first.impl().for_each_chunk(first.index(), last.index(),
                                std::forward<Fn>(fn));
}

template <typename T, typename Fn>
void for_each_chunk(const T* first, const T* last, Fn&& fn)
{
    std::forward<Fn>(fn)(first, last);
}

/*!
 * Equivalent of `std::accumulate` applied to the range `r`.
 */
template <typename Range, typename T>
T accumulate(Range&& r, T init)
{
    for_each_chunk(r, [&] (auto first, auto last) {
        init = std::accumulate(first, last, init);
    });
    return init;
}

/*!
 * Equivalent of `std::accumulate` applied to the range @f$ [first,
 * last) @f$.
 */
template <typename Iterator, typename T>
T accumulate(Iterator first, Iterator last, T init)
{
    for_each_chunk(first, last, [&] (auto first, auto last) {
        init = std::accumulate(first, last, init);
    });
    return init;
}

/*!
 * Equivalent of `std::for_each` applied to the range `r`.
 */
template <typename Range, typename Fn>
Fn&& for_each(Range&& r, Fn&& fn)
{
    for_each_chunk(r, [&] (auto first, auto last) {
        for (; first != last; ++first)
            fn(*first);
    });
    return std::forward<Fn>(fn);
}

/*!
 * Equivalent of `std::for_each` applied to the range @f$ [first,
 * last) @f$.
 */
template <typename Iterator, typename Fn>
Fn&& for_each(Iterator first, Iterator last, Fn&& fn)
{
    for_each_chunk(first, last, [&] (auto first, auto last) {
        for (; first != last; ++first)
            fn(*first);
    });
    return std::forward<Fn>(fn);
}

/*!
 * Equivalent of `std::copy` applied to the range `r`.
 */
template <typename Range, typename OutIter>
OutIter copy(Range&& r, OutIter out)
{
    for_each_chunk(r, [&] (auto first, auto last) {
        out = std::copy(first, last, out);
    });
    return out;
}

/*!
 * Equivalent of `std::copy` applied to the range @f$ [first,
 * last) @f$.
 */
template <typename InIter, typename OutIter>
OutIter copy(InIter first, InIter last, OutIter out)
{
    for_each_chunk(first, last, [&] (auto first, auto last) {
        out = std::copy(first, last, out);
    });
    return out;
}

/** @} */ // group: algorithm

} // namespace immer
