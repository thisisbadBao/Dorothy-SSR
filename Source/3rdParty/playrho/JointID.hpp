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

#ifndef PLAYRHO_JOINTID_HPP
#define PLAYRHO_JOINTID_HPP

/// @file
/// @brief Definition of the @c JointID alias and closely related code.

#include "playrho/detail/IndexingNamedType.hpp"
#include "playrho/Settings.hpp"

namespace playrho {

/// @brief Joint identifier.
using JointID = detail::IndexingNamedType<JointCounter, struct JointIdentifier>;

/// @brief Invalid joint ID value.
constexpr auto InvalidJointID = static_cast<JointID>(static_cast<JointID::underlying_type>(-1));

/// @brief Gets an invalid value for the JointID type.
template <>
constexpr JointID GetInvalid() noexcept
{
    return InvalidJointID;
}

/// @brief Determines if the given value is valid.
template <>
constexpr bool IsValid(const JointID& value) noexcept
{
    return value != GetInvalid<JointID>();
}

} // namespace playrho

#endif // PLAYRHO_JOINTID_HPP
