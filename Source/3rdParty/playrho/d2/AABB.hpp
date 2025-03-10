/*
 * Copyright (c) 2023 Louis Langholtz https://github.com/louis-langholtz/PlayRho
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

#ifndef PLAYRHO_D2_AABB_HPP
#define PLAYRHO_D2_AABB_HPP

/// @file
/// @brief Declaration of the AABB class and free functions that return instances of it.

#include "playrho/Intervals.hpp" // for LengthInterval, IsIntersecting
#include "playrho/Vector.hpp"
#include "playrho/Settings.hpp" // for ChildCounter, etc.
#include "playrho/Templates.hpp"
#include "playrho/BodyID.hpp"
#include "playrho/ShapeID.hpp"

#include "playrho/detail/AABB.hpp"

namespace playrho {

class Contact;

namespace detail {
template <std::size_t N> struct RayCastInput;
}

namespace d2 {

class Shape;
class DistanceProxy;
struct Transformation;
class World;

using ::playrho::detail::Contains;
using ::playrho::detail::TestOverlap;

/// @brief 2-Dimensional Axis Aligned Bounding Box.
using AABB = ::playrho::detail::AABB<2>;

/// @brief Gets the perimeter length of the 2-dimensional AABB.
/// @pre The sizes of each of the AABB's ranges are representable.
/// @return Twice the sum of the width and height.
/// @relatedalso playrho::detail::AABB
/// @see https://en.wikipedia.org/wiki/Perimeter
constexpr Length GetPerimeter(const AABB& aabb) noexcept
{
    return (GetSize(aabb.ranges[0]) + GetSize(aabb.ranges[1])) * 2;
}

/// @brief Computes the AABB.
/// @details Computes the Axis Aligned Bounding Box (AABB) for the given child shape
///   at a given a transform.
/// @param proxy Distance proxy for the child shape.
/// @param xf World transform of the shape.
/// @pre @p xf is valid.
/// @return AABB for the proxy shape or the default AABB if the proxy has a zero vertex count.
/// @relatedalso DistanceProxy
AABB ComputeAABB(const DistanceProxy& proxy, const Transformation& xf) noexcept;

/// @brief Computes the AABB.
/// @details Computes the Axis Aligned Bounding Box (AABB) for the given child shape
///   at the given transforms.
/// @param proxy Distance proxy for the child shape.
/// @param xfm0 World transform 0 of the shape.
/// @param xfm1 World transform 1 of the shape.
/// @pre @p xfm0 and @p xfm1 are both valid.
/// @return AABB for the proxy shape or the default AABB if the proxy has a zero vertex count.
/// @relatedalso DistanceProxy
AABB ComputeAABB(const DistanceProxy& proxy, const Transformation& xfm0,
                 const Transformation& xfm1) noexcept;

/// @brief Computes the AABB for the given shape with the given transformation.
/// @relatedalso Shape
AABB ComputeAABB(const Shape& shape, const Transformation& xf);

/// @brief Computes the AABB for the identified shape relative to the identified body
///   within the given world.
/// @relatedalso World
AABB ComputeAABB(const World& world, BodyID bodyID, ShapeID shapeID);

/// @brief Computes the AABB for the identified body within the given world.
/// @relatedalso World
AABB ComputeAABB(const World& world, BodyID id);

/// @brief Computes the intersecting AABB for the given pair of body-shape-index values.
/// @details The intersecting AABB for the given pair of body-shape-index values is the
///   intersection of the AABB for child A of shape A of body A with the AABB for child
///   B of shape B of body B.
/// @relatedalso World
AABB ComputeIntersectingAABB(const World& world, // force newline
                             BodyID bA, ShapeID sA, ChildCounter iA, // force newline
                             BodyID bB, ShapeID sB, ChildCounter iB);

/// @brief Computes the intersecting AABB for the given contact.
/// @relatedalso World
AABB ComputeIntersectingAABB(const World& world, const Contact& contact);

/// @brief Gets the AABB for the given ray cast input data.
/// @relatedalso playrho::detail::RayCastInput<2>
AABB GetAABB(const playrho::detail::RayCastInput<2>& input) noexcept;

} // namespace d2

/// @brief Gets an invalid AABB value.
/// @relatedalso detail::AABB
template <>
constexpr d2::AABB GetInvalid() noexcept
{
    return d2::AABB{LengthInterval{GetInvalid<Length>()}, LengthInterval{GetInvalid<Length>()}};
}

} // namespace playrho

#endif // PLAYRHO_D2_AABB_HPP
