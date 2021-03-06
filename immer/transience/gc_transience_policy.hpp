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

#include <immer/heap/tags.hpp>

#include <memory>

namespace immer {

/*!
 * Provides transience ownership tracking when a *tracing garbage
 * collector* is used instead of reference counting.
 *
 * @rst
 *
 * .. warning:: Using this policy without an allocation scheme that
 *    includes automatic tracing garbage collection may cause memory
 *    leaks.
 *
 * @endrst
 */
struct gc_transience_policy
{
    template <typename HeapPolicy>
    struct apply
    {
        struct type
        {
            using heap_ = typename HeapPolicy::type;

            struct edit
            {
                void* v;
                edit() = delete;
                bool operator==(edit x) const { return v == x.v; }
                bool operator!=(edit x) const { return v != x.v; }
            };

            struct owner
            {
                edit token_ {heap_::allocate(1, norefs_tag{})};

                operator edit () { return token_; }

                owner() {}
                owner(const owner&) {}
                owner(owner&& o) : token_{o.token_} {}
                owner& operator=(const owner&) { return *this; }
                owner& operator=(owner&& o)
                {
                    token_ = o.token_;
                    return *this;
                }
            };

            struct ownee
            {
                edit token_ {nullptr};

                ownee& operator=(edit e)
                {
                    assert(e != noone);
                    assert(token_ == e || token_ == edit{nullptr});
                    token_ = e;
                    return *this;
                }

                bool can_mutate(edit t) const { return token_ == t; }
                bool owned() const { return token_ != edit{nullptr}; }
            };

            static owner noone;
        };
    };
};

template <typename HP>
typename gc_transience_policy::apply<HP>::type::owner
gc_transience_policy::apply<HP>::type::noone = {};

} // namespace immer
