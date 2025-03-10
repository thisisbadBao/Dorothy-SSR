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

/**
 * @file
 * @brief Types and default settings file.
 * @note This file intentionally doesn't use any checked values to avoid
 *   imposing those for these base values.
 */

#ifndef PLAYRHO_SETTINGS_HPP
#define PLAYRHO_SETTINGS_HPP

#include <cstddef>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <algorithm>

#include "playrho/RealConstants.hpp"
#include "playrho/Templates.hpp"
#include "playrho/Units.hpp"
#include "playrho/WiderType.hpp"

namespace playrho {

/// @brief Maximum number of supportable edges in a simplex.
constexpr auto MaxSimplexEdges = std::uint8_t{3};

/// @brief Max child count.
constexpr auto MaxChildCount = std::numeric_limits<std::uint32_t>::max() >> 6;

/// @brief Child counter type.
/// @details Relating to "children" of shape where each child is a convex shape possibly
///   comprising a concave shape.
/// @note This type must always be able to contain the <code>MaxChildCount</code> value.
using ChildCounter = std::remove_const_t<decltype(MaxChildCount)>;

/// Time step iterations type.
/// @details A type for counting iterations per time-step.
using TimestepIters = std::uint8_t;

/// @brief Maximum float value.
constexpr auto MaxFloat = std::numeric_limits<Real>::max(); // FLT_MAX

// Collision

/// Maximum manifold points.
/// This is the maximum number of contact points between two convex shapes.
/// Do not change this value.
/// @note For memory efficiency, uses the smallest integral type that can hold the value.
constexpr auto MaxManifoldPoints = std::uint8_t{2};

/// @brief Maximum number of vertices for any shape type.
/// @note For memory efficiency, uses the smallest integral type that can hold the value minus
///   one that's left out as a sentinel value.
constexpr auto MaxShapeVertices = std::uint8_t{254};

/// @brief Vertex count type.
/// @note This type must not support more than 255 vertices as that would conflict
///   with the <code>ContactFeature::Index</code> type.
using VertexCounter = std::remove_const_t<decltype(MaxShapeVertices)>;

/// @brief Invalid vertex index.
constexpr auto InvalidVertex = static_cast<VertexCounter>(-1);

/// @brief Default linear slop.
/// @details Length used as a collision and constraint tolerance.
///   Usually chosen to be numerically significant, but visually insignificant.
///   Lower or raise to decrease or increase respectively the minimum of space
///   between bodies at rest.
/// @note Smaller values relative to sizes of bodies increases the time it takes
///   for bodies to come to rest.
/// @note The value used by Box2D 2.3.2 b2_linearSlop define is 0.005_m.
constexpr auto DefaultLinearSlop = 0.005_m;

/// @brief Default minimum vertex radius.
/// @note Recommend using <code>0.01_m</code> or <code>DefaultLinearSlop * Real(2)</code>.
constexpr auto DefaultMinVertexRadius = 0.01_m;

/// @brief Default maximum vertex radius.
/// @note Recommend using <code>255_m</code> or <code>DefaultLinearSlop * 2 * 25500</code>.
constexpr auto DefaultMaxVertexRadius = 255_m;

/// @brief Default AABB extension amount.
constexpr auto DefaultAabbExtension = DefaultLinearSlop * Real(20);

/// @brief Default distance multiplier.
constexpr auto DefaultDistanceMultiplier = Real(2);

/// @brief Default angular slop.
/// @details
/// A small angle used as a collision and constraint tolerance. Usually it is
/// chosen to be numerically significant, but visually insignificant.
constexpr auto DefaultAngularSlop = (Pi * 2_rad) / Real(180);

/// @brief Default maximum linear correction.
/// @details The maximum linear position correction used when solving constraints.
///   This helps to prevent overshoot.
/// @note This value should be greater than the linear slop value.
constexpr auto DefaultMaxLinearCorrection = 0.2_m;

/// @brief Default maximum angular correction.
/// @note This value should be greater than the angular slop value.
constexpr auto DefaultMaxAngularCorrection = Real(8.0f / 180.0f) * Pi * 1_rad;

/// @brief Default maximum translation amount.
constexpr auto DefaultMaxTranslation = 2_m;

/// @brief Default maximum rotation per world step.
/// @warning This value should always be less than 180 degrees - i.e. less than .5 * Pi * Radian.
/// @note This limit is meant to prevent numerical problems. Adjusting this value isn't advised.
/// @see StepConf::maxRotation.
constexpr auto DefaultMaxRotation = Angle{179_deg};

/// @brief Default maximum time of impact iterations.
constexpr auto DefaultMaxToiIters = std::uint8_t{20};

/// Default maximum time of impact root iterator count.
constexpr auto DefaultMaxToiRootIters = std::uint8_t{30};

/// Default max number of distance iterations.
constexpr auto DefaultMaxDistanceIters = std::uint8_t{20};

/// Default maximum number of sub steps.
/// @details
/// This is the default maximum number of sub-steps per contact in continuous physics simulation.
/// In other words, this is the default maximum number of times in a world step that a contact will
/// have continuous collision resolution done for it.
/// @note Used in the TOI phase of step processing.
constexpr auto DefaultMaxSubSteps = std::uint8_t{8};

// Dynamics

/// @brief Default velocity threshold.
constexpr auto DefaultVelocityThreshold = 1_mps;

/// @brief Default regular-phase minimum momentum.
constexpr auto DefaultRegMinMomentum = Momentum{0_Ns / 100};

/// @brief Default TOI-phase minimum momentum.
constexpr auto DefaultToiMinMomentum = Momentum{0_Ns / 100};

/// @brief Maximum number of bodies in a world.
/// @note This is 65534 based off <code>std::uint16_t</code> and eliminating one value for invalid.
constexpr auto MaxBodies = static_cast<std::uint16_t>(std::numeric_limits<std::uint16_t>::max() -
                                                      std::uint16_t{1});

/// @brief Count type for bodies.
/// @note This type must always be able to contain the <code>MaxBodies</code> value.
using BodyCounter = std::remove_const_t<decltype(MaxBodies)>;

/// @brief Count type for contacts.
/// @note This type must be able to contain the squared value of <code>BodyCounter</code>.
using ContactCounter = WiderType<BodyCounter>;

/// @brief Invalid contact index.
constexpr auto InvalidContactIndex = static_cast<ContactCounter>(-1);

/// @brief Maximum number of contacts in a world (2147319811).
/// @details Uses the formula for the maximum number of edges in an unidirectional graph of
///   <code>MaxBodies</code> nodes.
/// This occurs when every possible body is connected to every other body.
constexpr auto MaxContacts = ContactCounter{MaxBodies} * ContactCounter{MaxBodies - 1} / ContactCounter{2};

/// @brief Dynamic tree size type.
using DynamicTreeSize = ContactCounter;

/// @brief Maximum number of joints in a world.
/// @note This is 65534 based off <code>std::uint16_t</code> and eliminating one value for invalid.
constexpr auto MaxJoints = static_cast<std::uint16_t>(std::numeric_limits<std::uint16_t>::max() -
                                                      std::uint16_t{1});

/// @brief Counter type for joints.
/// @note This type must be able to contain the <code>MaxJoints</code> value.
using JointCounter = std::remove_const_t<decltype(MaxJoints)>;

/// @brief Maximum number of shapes in a world.
/// @note This is 65534 based off <code>std::uint16_t</code> and eliminating one value for invalid.
constexpr auto MaxShapes = static_cast<std::uint16_t>(std::numeric_limits<std::uint16_t>::max() -
                                                      std::uint16_t{1});

/// @brief Count type for shapes.
/// @note This type must always be able to contain the <code>MaxShapes</code> value.
using ShapeCounter = std::remove_const_t<decltype(MaxShapes)>;

/// @brief Default step time.
constexpr auto DefaultStepTime = Time{1_s / 60};

/// @brief Default step frequency.
constexpr auto DefaultStepFrequency = 60_Hz;

// Sleep

/// Default minimum still time to sleep.
/// @details The default minimum time bodies must be still for bodies to be put to sleep.
constexpr auto DefaultMinStillTimeToSleep = Time{1_s / 2}; // aka 0.5 secs

/// Default linear sleep tolerance.
/// @details A body cannot sleep if the magnitude of its linear velocity is above this amount.
constexpr auto DefaultLinearSleepTolerance = 0.01_mps; // aka 0.01

/// Default angular sleep tolerance.
/// @details A body cannot sleep if its angular velocity is above this amount.
constexpr auto DefaultAngularSleepTolerance = Real((Pi * 2) / 180) * RadianPerSecond;

/// Default circles ratio.
/// @details Ratio used for switching between rounded-corner collisions and closest-face
///   biased normal collisions.
constexpr auto DefaultCirclesRatio = Real(10);

} // namespace playrho

#endif // PLAYRHO_SETTINGS_HPP
