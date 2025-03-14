/*
 * Original work Copyright (c) 2006-2011 Erin Catto http://www.box2d.org
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

#ifndef PLAYRHO_D2_POSITIONSOLVERMANIFOLD_HPP
#define PLAYRHO_D2_POSITIONSOLVERMANIFOLD_HPP

/// @file
/// @brief Definition of the @c PositionSolverManifold class and closely related code.

#include "playrho/Settings.hpp"

#include "playrho/d2/Manifold.hpp"
#include "playrho/d2/Math.hpp"

namespace playrho {
namespace d2 {

/// Position solver manifold.
/// @details
/// This is a normal-point-separation composition of data for position constraint resolution.
struct PositionSolverManifold
{
    /// Normal.
    /// @details Normal of the contact between two points. This is the axis upon which impulses
    ///    should be applied to resolve the negative separations of overlapping shapes.
    UnitVec m_normal;

    /// Point.
    /// @details Point at which position resolution should be relatively applied.
    Length2 m_point{};
    
    /// Separation.
    /// @details Separation between two points (i.e. penetration if negative).
    Length m_separation{};
};

/// Gets the normal-point-separation data in world coordinates for the given inputs.
/// @note The returned normal is in the direction of shape A to shape B.
/// @note The returned separation distance does not account for vertex radiuses. It's simply
///   the separation between the points of the manifold. To account for the vertex radiuses,
///   the total vertex radius must be subtracted from this separation distance.
PositionSolverManifold GetPSM(const Manifold& manifold, Manifold::size_type index,
                              const Transformation& xfA, const Transformation& xfB);

} // namespace d2
} // namespace playrho

#endif // PLAYRHO_D2_POSITIONSOLVERMANIFOLD_HPP
