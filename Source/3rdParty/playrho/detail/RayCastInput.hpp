/*
 * Original work Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
 * Modified work Copyright (c) 2023 Louis Langholtz https://github.com/louis-langholtz/PlayRho
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef PLAYRHO_DETAIL_RAYCASTINPUT_HPP
#define PLAYRHO_DETAIL_RAYCASTINPUT_HPP

/// @file
/// @brief Declaration of the RayCastInput class template.

#include "playrho/Vector.hpp"
#include "playrho/UnitInterval.hpp"
#include "playrho/Units.hpp"

namespace playrho::detail {

/// @brief Ray-cast input data for N-dimensions.
/// @note The ray extends from <code>p1</code> to <code>p1 + maxFraction * (p2 - p1)</code>.
template <std::size_t N>
struct RayCastInput
{
    Vector<Length, N> p1; ///< Point 1.
    Vector<Length, N> p2; ///< Point 2.

    /// @brief Max fraction.
    /// @details Unit interval value - a value between 0 and 1 inclusive.
    UnitInterval<Real> maxFraction;
};

} // namespace playrho::detail

#endif // PLAYRHO_RAYCASTINPUT_HPP

