local Size = require("Size").Type
local Color = require("Color").Type

-- A record representing an application singleton instance.
local record App
	userdata

	-- The current passed frame number.
	const frame: integer

	-- The size of the main frame buffer texture used for rendering.
	const bufferSize: Size

	-- The logic visual size of the screen.
	-- The visual size only changes when application window size changes.
	-- And it won't be affacted by the view buffer scaling factor.
	const visualSize: Size

	-- The ratio of the pixel density displayed by the device
	-- Can be calculated as the size of the rendering buffer divided by the size of the application window.
	const devicePixelRatio: number

	-- An enumerated type representing the platform the game engine is running on.
	enum Platform
		"Windows"
		"Android"
		"macOS"
		"iOS"
		"Linux"
		"Unknown"
	end

	-- The platform the game engine is running on.
	const platform: Platform

	-- The version string of the game engine.
	-- Should be in format of "v0.0.0".
	const version: string

	-- The elapsed time since current frame was started, in seconds.
	const eclapsedTime: number

	-- The total time the game engine has been running until last frame ended, in seconds.
	-- Should be a contant number when invoked in a same frame for multiple times.
	const totalTime: number

	-- The total time the game engine has been running until this field being accessed, in seconds.
	-- Should be a increasing number when invoked in a same frame for multiple times.
	const runningTime: number

	-- A random number generated by a random number engine based on Mersenne Twister algorithm.
	-- So that the random number generated by a same seed should be consistent on every platform.
	const rand: integer

	-- The maximum valid frames per second the game engine is allowed to run at.
	-- The max FPS is being inferred by the device screen max refresh rate.
	const maxFPS: integer

	-- Whether the game engine is running in debug mode.
	const debugging: boolean

	-- An array of test names of engine included C++ tests.
	const testNames: {string}

	-- The system locale string, in format like: `zh-Hans`, `en`.
	locale: string

	-- The display orientation modes.
	enum Orientation
		"LandscapeLeft"
		"LandscapeRight"
		"Portrait"
		"PortraitUpsideDown"
	end

	-- The display orientation setting of the current device only takes effect on iOS and Android systems.
	orientation: Orientation

	-- A theme color for Dorothy SSR.
	themeColor: Color

	-- A random number seed.
	seed: integer

	-- The target frames per second the game engine is supposed to run at.
	-- Only works when `fpsLimited` is set to true.
	targetFPS: integer

	-- Whether the game engine is limiting the frames per second.
	-- Set `fpsLimited` to true, will make engine run in a busy loop to track the  precise frame time to switch to the next frame. And this behavior can lead to 100% CPU usage. This is usually common practice on Windows PCs for better CPU usage occupation. But it also results in extra heat and power consumption.
	fpsLimited: boolean

	-- Whether the game engine is currently idled.
	-- Set `idled` to true, will make game logic thread use a sleep time and going idled for next frame to come. Due to the imprecision in sleep time. This idled state may cause game engine over slept for a few frames to lost.
	-- `idled` state can reduce some CPU usage.
	idled: boolean

	-- The application window size.
	-- May differ from visual size due to the different DPIs of display devices.
	-- Set `winSize` to `Size.zero` to toggle application window into full screen mode,
	-- It is not available to set this property on platform Android and iOS.
	winSize: Size

	-- A function that runs an specific C++ test included in the engine.
	-- @param name (string) The name of the test to run.
	-- @return (boolean) Whether the test ran successfully.
	runTest: function(self: App, name: string): boolean

	-- A function that shuts down the game engine.
	-- It is not working and acts as a dummy function for platform Android and iOS to follow the specification of how mobile platform applications should operate.
	shutdown: function(self: App)
end

-- The global signal slot (event system) communication features.
-- This is just a demonstration record showing the global signal slot names and callbacks.
-- @usage
-- -- Register for these events using codes:
-- local node = Node():addTo(Director.entry)
-- node:gslot("AppQuit", function()
-- 	print("Application is shuting down!")
-- end)
local record __GSLOT__

	-- Triggers when the application is about to quit.
	["AppQuit"]: function()

	-- Triggers when the application receives a low memory warning.
	["AppLowMemory"]: function()

	-- Triggers when the application is about to enter the background.
	["AppWillEnterBackground"]: function()

	-- Triggers when the application has entered the background.
	["AppDidEnterBackground"]: function()

	-- Triggers when the application is about to enter the foreground.
	["AppWillEnterForeground"]: function()

	-- Triggers when the application has entered the foreground.
	["AppDidEnterForeground"]: function()

	-- Triggers when the application window size changes.
	["AppSizeChanged"]: function()

	-- Triggers when the application window enters or exits full-screen mode.
	["AppFullScreen"]: function(fullScreen: boolean)

	-- Triggers when the application theme color changes.
	["AppTheme"]: function(themeColor: Color)
end

local app: App
return app
