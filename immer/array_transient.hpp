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

#include <immer/memory_policy.hpp>
#include <immer/detail/arrays/with_capacity.hpp>

namespace immer {

template <typename T, typename MemoryPolicy>
class array;

/*!
 * Mutable version of `immer::array`.
 *
 * @rst
 *
 * Refer to :doc:`transients` to learn more about when and how to use
 * the mutable versions of immutable containers.
 *
 * @endrst
 */
template <typename T, typename MemoryPolicy = default_memory_policy>
class array_transient
    : MemoryPolicy::transience_t::owner
{
    using impl_t = detail::arrays::with_capacity<T, MemoryPolicy>;
    using impl_no_capacity_t = detail::arrays::no_capacity<T, MemoryPolicy>;

public:
    using value_type = T;
    using reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using const_reference = const T&;

    using iterator         = const T*;
    using const_iterator   = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;

    using memory_policy   = MemoryPolicy;
    using persistent_type = array<T, MemoryPolicy>;

    /*!
     * Default constructor.  It creates a mutable array of `size() ==
     * 0`.  It does not allocate memory and its complexity is
     * @f$ O(1) @f$.
     */
    array_transient() = default;

    /*!
     * Returns an iterator pointing at the first element of the
     * collection. It does not allocate memory and its complexity is
     * @f$ O(1) @f$.
     */
    iterator begin() const { return impl_.data(); }

    /*!
     * Returns an iterator pointing just after the last element of the
     * collection. It does not allocate and its complexity is @f$ O(1) @f$.
     */
    iterator end()   const { return impl_.data() + impl_.size; }

    /*!
     * Returns an iterator that traverses the collection backwards,
     * pointing at the first element of the reversed collection. It
     * does not allocate memory and its complexity is @f$ O(1) @f$.
     */
    reverse_iterator rbegin() const { return reverse_iterator{end()}; }

    /*!
     * Returns an iterator that traverses the collection backwards,
     * pointing after the last element of the reversed collection. It
     * does not allocate memory and its complexity is @f$ O(1) @f$.
     */
    reverse_iterator rend()   const { return reverse_iterator{begin()}; }

    /*!
     * Returns the number of elements in the container.  It does
     * not allocate memory and its complexity is @f$ O(1) @f$.
     */
    std::size_t size() const { return impl_.size; }

    /*!
     * Returns `true` if there are no elements in the container.  It
     * does not allocate memory and its complexity is @f$ O(1) @f$.
     */
    bool empty() const { return impl_.d->empty(); }

    /*!
     * Access the raw data.
     */
    const T* data() const { return impl_.data(); }

    /*!
     * Access the last element.
     */
    const T& back() const { return data()[size() - 1]; }

    /*!
     * Access the first element.
     */
    const T& front() const { return data()[0]; }

    /*!
     * Returns a `const` reference to the element at position `index`.
     * It does not allocate memory and its complexity is @f$ O(1) @f$.
     */
    reference operator[] (size_type index) const
    { return impl_.get(index); }

    /*!
     * Inserts `value` at the end.  It may allocate memory and its
     * complexity is *effectively* @f$ O(1) @f$.
     */
    void push_back(value_type value)
    { impl_.push_back_mut(*this, std::move(value)); }

    /*!
     * Sets to the value `value` at position `idx`.
     * Undefined for `index >= size()`.
     * It may allocate memory and its complexity is
     * *effectively* @f$ O(1) @f$.
     */
    void set(size_type index, value_type value)
    { impl_.assoc_mut(*this, index, std::move(value)); }

    /*!
     * Updates the array to contain the result of the expression
     * `fn((*this)[idx])` at position `idx`.
     * Undefined for `0 >= size()`.
     * It may allocate memory and its complexity is
     * *effectively* @f$ O(1) @f$.
     */
    template <typename FnT>
    void update(size_type index, FnT&& fn)
    { impl_.update_mut(*this, index, std::forward<FnT>(fn)); }

    /*!
     * Resizes the array to only contain the first `min(elems, size())`
     * elements. It may allocate memory and its complexity is
     * *effectively* @f$ O(1) @f$.
     */
    void take(size_type elems)
    { impl_.take_mut(*this, elems); }

    /*!
     * Returns an @a immutable form of this container, an
     * `immer::array`.
     */
    persistent_type persistent() const&
    { return persistent_type{ impl_ }; }
    persistent_type persistent() &&
    { return persistent_type{ std::move(impl_) }; }

private:
    friend persistent_type;

    array_transient(impl_t impl)
        : impl_(std::move(impl))
    {}

    impl_t  impl_  = impl_t::empty;
};

} // namespace immer
