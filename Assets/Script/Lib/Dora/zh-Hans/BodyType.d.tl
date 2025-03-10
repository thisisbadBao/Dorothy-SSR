local Node = require("Node").Type
local BodyDef = require("BodyDef").Type
local Vec2 = require("Vec2").Type
local Object = require("Object").Type
local FixtureDef = require("FixtureDef").Type
local type Sensor = require("SensorType")
local type PhysicsWorld = require("PhysicsWorldType")

-- 物理世界中的物理体类。
local record Body

	-- 继承自“Node”。
	embed Node

	-- 物理体所属的物理世界。
	const world: PhysicsWorld

	-- 物理体的定义。
	const bodyDef: BodyDef

	-- 物理体的质量。
	const mass: number

	-- 物理体是否被用作传感器。
	const sensor: boolean

	-- 物理体的 x 轴速度。
	velocityX: number

	-- 物理体的 y 轴速度。
	velocityY: number

	-- “Vec2”对象表示的物理体的速度。
	velocity: Vec2

	-- 物理体的角速率。
	angularRate: number

	-- 物理体所属的碰撞组编号。
	group: integer

	-- 物理体的线性阻尼。
	linearDamping: number

	-- 物理体的旋转阻尼。
	angularDamping: number

	-- 物理体的所有者。
	owner: Object

	-- 物理体当前是否会接收碰撞接触事件。
	receivingContact: boolean

	-- 在指定位置对物理体施加线性冲量。
	-- @param impulse (Vec2) 要施加的线性冲量。
	-- @param pos (Vec2) 施加冲量的位置。
	applyLinearImpulse: function(self: Body, impulse: Vec2, pos: Vec2)

	-- 对物理体施加角冲量。
	-- @param impulse (number) 要施加的角冲量。
	applyAngularImpulse: function(self: Body, impulse: number)

	-- 从物理体中移除具有指定标签的传感器。
	-- @param tag (integer) 要移除的传感器的标签编号。
	-- @return (boolean) 是否找到并删除具有指定标签的传感器。
	removeSensorByTag: function(self: Body, tag: integer): boolean

	-- 将混合物添加到物理体上。
	-- @param fixtureDef (FixtureDef) 要添加的混合物的定义。
	attach: function(self: Body, fixtureDef: FixtureDef)

	-- 获取具有特定标签编号的传感器。
	-- @param tag (integer) 要获取的传感器的标签编号。
	-- @return (Sensor) 具有给定标签的传感器。
	getSensorByTag: function(self: Body, tag: integer): Sensor

	-- 从物理体的传感器列表中移除特定的传感器。
	-- @param sensor (Sensor) 要移除的传感器。
	-- @return (boolean) 如果成功删除传感器，则为true，否则为false。
	removeSensor: function(self: Body, sensor: Sensor): boolean

	-- 将具有特定标签编号和混合物定义的新传感器添加到物理体上。
	-- @param tag (integer) 要添加的传感器的标签编号。
	-- @param fixtureDef (FixtureDef) 传感器的混合物定义。
	-- @return (Sensor) 新建的传感器。
	attachSensor: function(self: Body, tag: integer, fixtureDef: FixtureDef): Sensor
end

return Body
