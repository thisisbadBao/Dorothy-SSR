/* Copyright (c) 2023 Jin Li, dragon-fly@qq.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#pragma once

#include "Common/Async.h"
#include "Input/TouchDispather.h"

struct ImDrawData;
struct ImFontAtlas;

NS_DOROTHY_BEGIN

class Listener;
class Texture2D;
class Pass;
class ConsolePanel;

class ImGuiDora {
public:
	PROPERTY_BOOL(FontLoaded);
	virtual ~ImGuiDora();
	bool init();
	void begin();
	void end();
	void render();
	void loadFontTTFAsync(String ttfFontFile, float fontSize, String glyphRanges, const std::function<void(bool)>& handler);
	void showStats(const std::function<void()>& extra = nullptr);
	void showConsole();
	void handleEvent(const SDL_Event& event);
	void updateTexture(uint8_t* data, int width, int height);

	class ImGuiTouchHandler : public TouchHandler {
	public:
		ImGuiTouchHandler(ImGuiDora* owner)
			: _owner(owner) { }
		virtual ~ImGuiTouchHandler() { }
		virtual bool handle(const SDL_Event& event) override;

	protected:
		ImGuiDora* _owner;
	};
	PROPERTY_READONLY(ImGuiTouchHandler*, TouchHandler);

public:
	static void setImePositionHint(int x, int y);

protected:
	ImGuiDora();
	void sendKey(int key, int count);
	static const char* getClipboardText(void*);
	static void setClipboardText(void*, const char* text);
	static int _lastIMEPosX, _lastIMEPosY;

private:
	bool _fontLoaded;
	bool _showPlot;
	uint32_t _objectFrames;
	uint32_t _timeFrames;
	uint32_t _memFrames;
	uint32_t _profileFrames;
	double _objectEclapsed;
	double _memEclapsed;
	double _profileEclapsed;
	double _cpuTime;
	double _gpuTime;
	double _deltaTime;
	double _avgCPUTime;
	double _avgGPUTime;
	double _avgDeltaTime;
	double _logicTime;
	double _renderTime;
	int _memPoolSize;
	int _memLua;
	int _memWASM;
	int _lastMemPoolSize;
	int _lastMemLua;
	int _lastMemWASM;
	std::vector<double> _cpuValues;
	std::vector<double> _gpuValues;
	std::vector<double> _dtValues;
	std::vector<double> _times;
	double _maxCPU;
	double _maxGPU;
	double _maxDelta;
	double _yLimit;
	uint32_t _maxCppObjects;
	uint32_t _maxLuaObjects;
	uint32_t _maxCallbacks;

private:
	bool _isChineseSupported;
	bool _useChinese;
	bool _isLoadingFont;
	bool _textInputing;
	bool _backSpaceIgnore;
	bool _mouseVisible;
	bool _mousePressed[3];
	bool _rejectAllEvents;
	Vec2 _mouseWheel;
	int _lastCursor;
	Ref<Texture2D> _fontTexture;
	bgfx::UniformHandle _sampler;
	Ref<Pass> _defaultPass;
	Ref<Pass> _imagePass;
	Ref<Listener> _costListener;
	Ref<Listener> _themeListener;
	Ref<Listener> _localeListener;
	bgfx::VertexLayout _vertexLayout;
	std::list<std::any> _inputs;
	std::vector<uint32_t> _textEditing;
	std::string _iniFilePath;
	Own<ConsolePanel> _console;
	Own<ImFontAtlas> _defaultFonts;
	Own<ImFontAtlas> _fonts;
	std::shared_ptr<ImGuiTouchHandler> _touchHandler;
	std::unordered_map<std::string, double> _timeCosts;
	std::unordered_map<std::string, double> _updateCosts;
	struct LoaderCost {
		int id;
		int level;
		std::string module;
		double time;
		std::string levelStr;
		std::string timeStr;
	};
	double _loaderTotalTime;
	std::deque<LoaderCost> _loaderCosts;
	std::unordered_map<int, int> _keymap;
	SINGLETON_REF(ImGuiDora, BGFXDora);
	// font building is calling in thread, so make thread depend on ImGui
	SINGLETON_REF(AsyncThread, ImGuiDora);
};

#define SharedImGui \
	Dorothy::Singleton<Dorothy::ImGuiDora>::shared()

NS_DOROTHY_END
