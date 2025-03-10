/* Copyright (c) 2023 Jin Li, dragon-fly@qq.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#pragma once

#include "Support/Array.h"
#include "Support/Common.h"
#include "Support/Geometry.h"

union SDL_Event;
struct NVGcontext;

NS_DOROTHY_BEGIN

class Scheduler;
class Node;
class Sprite;
class Camera;
class RenderTarget;
class UITouchHandler;

class Director {
public:
	virtual ~Director();
	PROPERTY(Scheduler*, Scheduler);
	PROPERTY(Color, ClearColor);
	PROPERTY_READONLY_CALL(Node*, UI);
	PROPERTY_READONLY_CALL(Node*, UI3D);
	PROPERTY_READONLY_CALL(Node*, Entry);
	PROPERTY_READONLY_CALL(Node*, PostNode);
	PROPERTY_READONLY_CALL(UITouchHandler*, UITouchHandler);
	PROPERTY_READONLY_CALL(Camera*, CurrentCamera);
	PROPERTY_READONLY(Camera*, PrevCamera);
	PROPERTY_READONLY(Scheduler*, SystemScheduler);
	PROPERTY_READONLY(Scheduler*, PostScheduler);
	PROPERTY_READONLY(double, DeltaTime);
	PROPERTY_READONLY(const Matrix&, ViewProjection);
	bool init();
	void doLogic();
	void doRender();
	void handleSDLEvent(const SDL_Event& event);

	void pushCamera(Camera* camera);
	void popCamera();
	bool removeCamera(Camera* camera);
	void clearCamera();

	void markDirty();
	NVGcontext* markNVGDirty();

	void cleanup();

	template <typename Func>
	void pushViewProjection(const Matrix& viewProj, const Func& workHere) {
		pushViewProjection(viewProj);
		workHere();
		popViewProjection();
	}

protected:
	Director();
	void pushViewProjection(const Matrix& viewProj);
	void popViewProjection();

private:
	bool _nvgDirty;
	bool _paused;
	bool _stoped;
	Color _clearColor;
	Ref<Node> _ui;
	Ref<Node> _ui3D;
	Ref<Camera> _uiCamera;
	Ref<Camera> _ui3DCamera;
	Ref<Node> _postNode;
	Ref<Node> _entry;
	Ref<Node> _root;
	Ref<Array> _camStack;
	Ref<Scheduler> _systemScheduler;
	Ref<Scheduler> _scheduler;
	Ref<Scheduler> _postScheduler;
	Own<UITouchHandler> _uiTouchHandler;
	std::stack<Own<Matrix>> _viewProjs;
	Matrix _defaultViewProj;
	NVGcontext* _nvgContext;
	SINGLETON_REF(Director, FontManager, LuaEngine, BGFXDora, Application);
};

#define SharedDirector \
	Dorothy::Singleton<Dorothy::Director>::shared()

NS_DOROTHY_END
