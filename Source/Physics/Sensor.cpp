/* Copyright (c) 2023 Jin Li, dragon-fly@qq.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "Const/Header.h"

#include "Physics/Sensor.h"

#include "Physics/Body.h"
#include "Physics/PhysicsWorld.h"
#include "Support/Array.h"

NS_DOROTHY_BEGIN

Sensor::Sensor(Body* owner, int tag, pr::ShapeID fixture)
	: _owner(owner)
	, _tag(tag)
	, _fixture(fixture)
	, _enabled(true)
	, _sensedBodies(Array::create()) { }

Sensor::~Sensor() {
	Sensor::clear();
	_fixture = pr::InvalidShapeID;
}

void Sensor::add(Body* body) {
	_sensedBodies->add(Value::alloc(body));
	bool success;
	std::tie(std::ignore, success) = _uniqueBodies.insert(body);
	if (success && bodyEnter) {
		bodyEnter(body, _tag);
	}
}

void Sensor::remove(Body* body) {
	auto value = Value::alloc(body);
	_sensedBodies->fastRemove(value.get());
	if (_uniqueBodies.erase(body) > 0 && bodyLeave) {
		bodyLeave(body, _tag);
	}
}

bool Sensor::contains(Body* body) {
	ARRAY_START(Body, item, _sensedBodies) {
		if (item == body) {
			return true;
		}
	}
	ARRAY_END
	return false;
}

void Sensor::clear() {
	_uniqueBodies.clear();
	_sensedBodies->clear();
}

Array* Sensor::getSensedBodies() const {
	return _sensedBodies;
}

void Sensor::setEnabled(bool enable) {
	_enabled = enable;
	if (!enable) {
		Sensor::clear();
	}
}

bool Sensor::isEnabled() const {
	return _enabled;
}

int Sensor::getTag() const {
	return _tag;
}

void Sensor::setGroup(int var) {
	auto world = _owner->getPhysicsWorld();
	pd::SetFilterData(world->getPrWorld(), _fixture, world->getFilter(var));
}

int Sensor::getGroup() const {
	auto world = _owner->getPhysicsWorld();
	return pd::GetFilterData(world->getPrWorld(), _fixture).groupIndex;
}

Body* Sensor::getOwner() const {
	return _owner;
}

pr::ShapeID Sensor::getFixture() const {
	return _fixture;
}

bool Sensor::isSensed() const {
	return _sensedBodies->getCount() > 0;
}

NS_DOROTHY_END
