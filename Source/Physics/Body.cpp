/* Copyright (c) 2023 Jin Li, dragon-fly@qq.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "Const/Header.h"

#include "Physics/Body.h"

#include "Physics/BodyDef.h"
#include "Physics/PhysicsWorld.h"
#include "Physics/Sensor.h"

NS_DOROTHY_BEGIN

Body::Body(BodyDef* bodyDef, PhysicsWorld* world, const Vec2& pos, float rot)
	: _prBody(pr::InvalidBodyID)
	, _bodyDef(bodyDef)
	, _pWorld(world)
	, _group(0) {
	AssertIf(world == nullptr, "init Body with invalid PhysicsWorld.");
	bodyDef->getConf()->UseLocation(PhysicsWorld::prVal(pos + bodyDef->offset));
	bodyDef->getConf()->UseAngle(-bx::toRad(rot + bodyDef->angleOffset));
}

Body::~Body() {
	if (_prBody != pr::InvalidBodyID) {
		pd::Destroy(_pWorld->getPrWorld(), _prBody);
		_pWorld->setBodyData(_prBody, nullptr);
		_prBody = pr::InvalidBodyID;
	}
	ARRAY_START(Sensor, sensor, _sensors) {
		sensor->bodyEnter.Clear();
		sensor->bodyLeave.Clear();
	}
	ARRAY_END
	contactStart.Clear();
	contactEnd.Clear();
}

bool Body::init() {
	if (!Node::init()) return false;
	_prBody = pd::CreateBody(_pWorld->getPrWorld(), *_bodyDef->getConf());
	_pWorld->setBodyData(_prBody, this);
	Node::setPosition(PhysicsWorld::Val(_bodyDef->getConf()->location));
	for (BodyDef::FixtureConf& fixtureConf : _bodyDef->getFixtureConfs()) {
		if (pd::IsSensor(fixtureConf.shape)) {
			Body::attachSensor(fixtureConf.tag, fixtureConf.shape);
		} else {
			Body::attachFixture(fixtureConf.shape);
		}
	}
	contactStart += std::make_pair(this, &Body::onContactStart);
	contactEnd += std::make_pair(this, &Body::onContactEnd);
	return true;
}

void Body::onEnter() {
	Node::onEnter();
	pd::SetEnabled(_pWorld->getPrWorld(), _prBody, true);
}

void Body::onExit() {
	Node::onExit();
	pd::SetEnabled(_pWorld->getPrWorld(), _prBody, false); // Set enable false to trigger sensor`s body leave event.
}

void Body::cleanup() {
	if (_flags.isOff(Node::Cleanup)) {
		Node::cleanup();
		if (_prBody != pr::InvalidBodyID) {
			pd::Destroy(_pWorld->getPrWorld(), _prBody);
			_pWorld->setBodyData(_prBody, nullptr);
			_prBody = pr::InvalidBodyID;
		}
		if (_sensors) {
			ARRAY_START(Sensor, sensor, _sensors) {
				_pWorld->setFixtureData(sensor->getFixture(), nullptr);
				sensor->bodyEnter.Clear();
				sensor->bodyLeave.Clear();
				sensor->setEnabled(false);
				sensor->getSensedBodies()->clear();
			}
			ARRAY_END
			_sensors->clear();
		}
		contactStart.Clear();
		contactEnd.Clear();
	}
}

BodyDef* Body::getBodyDef() const {
	return _bodyDef;
}

PhysicsWorld* Body::getPhysicsWorld() const {
	return _pWorld;
}

pr::BodyID Body::getPrBody() const {
	return _prBody;
}

Sensor* Body::getSensorByTag(int tag) {
	ARRAY_START(Sensor, sensor, _sensors) {
		if (sensor->getTag() == tag) {
			return sensor;
		}
	}
	ARRAY_END
	return nullptr;
}

bool Body::removeSensorByTag(int tag) {
	Sensor* sensor = Body::getSensorByTag(tag);
	return Body::removeSensor(sensor);
}

bool Body::removeSensor(Sensor* sensor) {
	auto& world = _pWorld->getPrWorld();
	auto sensorRef = Value::alloc(sensor);
	if (_sensors && sensor && _sensors->remove(sensorRef.get())) {
		pd::Destroy(world, sensor->getFixture());
		_pWorld->setFixtureData(sensor->getFixture(), nullptr);
		return true;
	}
	return false;
}

void Body::setVelocity(float x, float y) {
	auto& world = _pWorld->getPrWorld();
	pd::SetVelocity(world, _prBody, pr::LinearVelocity2{PhysicsWorld::prVal(x), PhysicsWorld::prVal(y)});
}

void Body::setVelocity(const Vec2& velocity) {
	Body::setVelocity(velocity.x, velocity.y);
}

Vec2 Body::getVelocity() const {
	auto& world = _pWorld->getPrWorld();
	return PhysicsWorld::Val(pd::GetVelocity(world, _prBody).linear);
}

void Body::setAngularRate(float var) {
	auto& world = _pWorld->getPrWorld();
	pd::SetVelocity(world, _prBody, -bx::toRad(var));
}

float Body::getAngularRate() const {
	auto& world = _pWorld->getPrWorld();
	return -bx::toDeg(pd::GetAngularVelocity(world, _prBody));
}

void Body::setLinearDamping(float var) {
	auto& world = _pWorld->getPrWorld();
	pd::SetLinearDamping(world, _prBody, var);
}

float Body::getLinearDamping() const {
	auto& world = _pWorld->getPrWorld();
	return pd::GetLinearDamping(world, _prBody);
}

void Body::setAngularDamping(float var) {
	auto& world = _pWorld->getPrWorld();
	pd::SetAngularDamping(world, _prBody, var);
}

float Body::getAngularDamping() const {
	auto& world = _pWorld->getPrWorld();
	return pd::GetAngularDamping(world, _prBody);
}

void Body::setOwner(Object* owner) {
	_owner = owner;
}

Object* Body::getOwner() const {
	return _owner;
}

float Body::getMass() const {
	auto& world = _pWorld->getPrWorld();
	return 1.0f / pd::GetInvMass(world, _prBody);
}

void Body::setGroup(uint8_t group) {
	AssertIf(group >= PhysicsWorld::TotalGroups, "Body group should be less than {}.", s_cast<int>(PhysicsWorld::TotalGroups));
	_group = group;
	auto& world = _pWorld->getPrWorld();
	const auto& filter = _pWorld->getFilter(group);
	for (pr::ShapeID f : pd::GetShapes(world, _prBody)) {
		pd::SetFilterData(world, f, filter);
	}
}

uint8_t Body::getGroup() const {
	return _group;
}

void Body::applyLinearImpulse(const Vec2& impulse, const Vec2& pos) {
	auto& world = _pWorld->getPrWorld();
	pd::ApplyLinearImpulse(world, _prBody, PhysicsWorld::prVal(impulse), PhysicsWorld::prVal(pos));
}

void Body::applyAngularImpulse(float impulse) {
	auto& world = _pWorld->getPrWorld();
	pd::ApplyAngularImpulse(world, _prBody, PhysicsWorld::prVal(impulse));
}

pr::ShapeID Body::attachFixture(const pd::Shape& shape) {
	auto& world = _pWorld->getPrWorld();
	pr::ShapeID fixture = pd::CreateShape(
		world,
		shape);
	pd::Attach(world, _prBody, fixture);
	return fixture;
}

pr::ShapeID Body::attach(FixtureDef* fixtureDef) {
	pr::ShapeID fixture = Body::attachFixture(fixtureDef->shape);
	/* cleanup temp vertices */
	if (pd::GetType(fixtureDef->shape) == pr::GetTypeID<pd::ChainShapeConf>()) {
		fixtureDef->shape = pd::Shape{};
	}
	return fixture;
}

Sensor* Body::attachSensor(int tag, pd::Shape& shape) {
	pd::SetSensor(shape, true);
	pd::SetFilter(shape, _pWorld->getFilter(_group));
	auto& world = _pWorld->getPrWorld();
	pr::ShapeID fixture = pd::CreateShape(world, shape);
	pd::Attach(world, _prBody, fixture);
	Sensor* sensor = Sensor::create(this, tag, fixture);
	_pWorld->setFixtureData(fixture, sensor);
	sensor->bodyEnter += std::make_pair(this, &Body::onBodyEnter);
	sensor->bodyLeave += std::make_pair(this, &Body::onBodyLeave);
	if (!_sensors) _sensors = Array::create();
	_sensors->add(Value::alloc(sensor));
	return sensor;
}

Sensor* Body::attachSensor(int tag, FixtureDef* fixtureDef) {
	return Body::attachSensor(tag, fixtureDef->shape);
}

bool Body::isSensor() const {
	return _sensors && _sensors->getCount() > 0;
}

void Body::setVelocityX(float x) {
	auto& world = _pWorld->getPrWorld();
	auto v = pd::GetVelocity(world, _prBody).linear;
	pd::SetVelocity(world, _prBody, pr::LinearVelocity2{PhysicsWorld::prVal(x), v[1]});
}

float Body::getVelocityX() const {
	auto& world = _pWorld->getPrWorld();
	return PhysicsWorld::Val(pd::GetVelocity(world, _prBody).linear[0]);
}

void Body::setVelocityY(float y) {
	auto& world = _pWorld->getPrWorld();
	auto v = pd::GetVelocity(world, _prBody).linear;
	pd::SetVelocity(world, _prBody, pr::LinearVelocity2{v[0], PhysicsWorld::prVal(y)});
}

float Body::getVelocityY() const {
	auto& world = _pWorld->getPrWorld();
	return PhysicsWorld::Val(pd::GetVelocity(world, _prBody).linear[1]);
}

void Body::setPosition(const Vec2& var) {
	if (var != Node::getPosition()) {
		Node::setPosition(var);
		auto& world = _pWorld->getPrWorld();
		pd::SetTransform(world, _prBody, PhysicsWorld::prVal(var), pd::GetAngle(world, _prBody));
	}
}

void Body::setAngle(float var) {
	if (var != Node::getAngle()) {
		Node::setAngle(var);
		auto& world = _pWorld->getPrWorld();
		pd::SetTransform(world, _prBody, pd::GetLocation(world, _prBody), -bx::toRad(var));
	}
}

Rect Body::getBoundingBox() {
	auto& world = _pWorld->getPrWorld();
	pd::AABB aabb = pd::ComputeAABB(world, _prBody);
	Vec2 lower = PhysicsWorld::Val(pr::detail::GetLowerBound(aabb));
	Vec2 upper = PhysicsWorld::Val(pr::detail::GetUpperBound(aabb));
	return Rect(lower.x, lower.y, upper.x - lower.x, upper.y - lower.y);
}

void Body::setReceivingContact(bool var) {
	_flags.set(Body::ReceivingContact, var);
}

bool Body::isReceivingContact() const {
	return _flags.isOn(Body::ReceivingContact);
}

void Body::onBodyEnter(Body* other, int sensorTag) {
	emit("BodyEnter"_slice, other, sensorTag);
}

void Body::onBodyLeave(Body* other, int sensorTag) {
	emit("BodyLeave"_slice, other, sensorTag);
}

void Body::onContactStart(Body* other, const Vec2& point, const Vec2& normal) {
	emit("ContactStart"_slice, other, point, normal);
}

void Body::onContactEnd(Body* other, const Vec2& point, const Vec2& normal) {
	emit("ContactEnd"_slice, other, point, normal);
}

void Body::updatePhysics() {
	auto& world = _pWorld->getPrWorld();
	if (pd::IsAwake(world, _prBody)) {
		Vec2 pos = PhysicsWorld::Val(pd::GetLocation(world, _prBody));
		/* Here only Node::setPosition(const Vec2& var) work for modify Node`s position.
		 Other positioning functions have been overriden by Body`s.
		*/
		Node::setPosition(pos);
		float angle = pd::GetAngle(world, _prBody);
		Node::setAngle(-bx::toDeg(angle));
	}
}

NS_DOROTHY_END
