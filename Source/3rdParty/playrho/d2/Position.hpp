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

#ifndef PLAYRHO_D2_POSITION_HPP
#define PLAYRHO_D2_POSITION_HPP

/// @file
/// @brief Definition of the @c Position class and closely related code.

#include "playrho/Settings.hpp"
#include "playrho/Templates.hpp"
#include "playrho/Vector2.hpp"

namespace playrho {
namespace d2 {

/// @brief 2-D positional data structure.
/// @details A 2-element length and angle pair suitable for representing a linear and
///   angular position in 2-D.
struct Position {
    Length2 linear; ///< Linear position.
    Angle angular; ///< Angular position.
};

/// @brief Equality operator.
/// @relatedalso Position
constexpr bool operator==(const Position& lhs, const Position& rhs)
{
    return (lhs.linear == rhs.linear) && (lhs.angular == rhs.angular);
}

/// @brief Inequality operator.
/// @relatedalso Position
constexpr bool operator!=(const Position& lhs, const Position& rhs)
{
    return (lhs.linear != rhs.linear) || (lhs.angular != rhs.angular);
}

/// @brief Negation operator.
/// @relatedalso Position
constexpr Position operator-(const Position& value)
{
    return Position{-value.linear, -value.angular};
}

/// @brief Positive operator.
/// @relatedalso Position
constexpr Position operator+(const Position& value)
{
    return value;
}

/// @brief Addition assignment operator.
/// @relatedalso Position
constexpr Position& operator+=(Position& lhs, const Position& rhs)
{
    lhs.linear += rhs.linear;
    lhs.angular += rhs.angular;
    return lhs;
}

/// @brief Addition operator.
/// @relatedalso Position
constexpr Position operator+(const Position& lhs, const Position& rhs)
{
    return Position{lhs.linear + rhs.linear, lhs.angular + rhs.angular};
}

/// @brief Subtraction assignment operator.
/// @relatedalso Position
constexpr Position& operator-=(Position& lhs, const Position& rhs)
{
    lhs.linear -= rhs.linear;
    lhs.angular -= rhs.angular;
    return lhs;
}

/// @brief Subtraction operator.
/// @relatedalso Position
constexpr Position operator-(const Position& lhs, const Position& rhs)
{
    return Position{lhs.linear - rhs.linear, lhs.angular - rhs.angular};
}

/// @brief Multiplication operator.
constexpr Position operator*(const Position& pos, const Real scalar)
{
    return Position{pos.linear * scalar, pos.angular * scalar};
}

/// @brief Multiplication operator.
/// @relatedalso Position
constexpr Position operator*(const Real scalar, const Position& pos)
{
    return Position{pos.linear * scalar, pos.angular * scalar};
}

} // namespace d2

/// @brief Determines if the given value is valid.
/// @relatedalso d2::Position
template <>
constexpr bool IsValid(const d2::Position& value) noexcept
{
    return IsValid(value.linear) && IsValid(value.angular);
}

} // namespace playrho

#endif // PLAYRHO_D2_POSITION_HPP
