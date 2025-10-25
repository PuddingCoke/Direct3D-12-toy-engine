#include<Gear/Input/Mouse.h>

#include<Gear/Input/Internal/MouseInternal.h>

namespace
{
	struct MousePrivate
	{

		float x = 0.f;

		float y = 0.f;

		float dx = 0.f;

		float dy = 0.f;

		float wheelDelta = 0.f;

		bool leftDown = false;

		bool rightDown = false;

		bool onMoved = false;

		bool onLeftDowned = false;

		bool onRightDowned = false;

		bool onScrolled = false;

		Input::Event moveEvent;

		Input::Event leftDownEvent;

		Input::Event rightDownEvent;

		Input::Event leftUpEvent;

		Input::Event rightUpEvent;

		Input::Event scrollEvent;

	}pvt;
}

float Input::Mouse::getX()
{
	return pvt.x;
}

float Input::Mouse::getY()
{
	return pvt.y;
}

float Input::Mouse::getDX()
{
	return pvt.dx;
}

float Input::Mouse::getDY()
{
	return pvt.dy;
}

float Input::Mouse::getWheelDelta()
{
	return pvt.wheelDelta;
}

bool Input::Mouse::getLeftDown()
{
	return pvt.leftDown;
}

bool Input::Mouse::getRightDown()
{
	return pvt.rightDown;
}

bool Input::Mouse::onMove()
{
	return pvt.onMoved;
}

bool Input::Mouse::onLeftDown()
{
	return pvt.onLeftDowned;
}

bool Input::Mouse::onRightDown()
{
	return pvt.onRightDowned;
}

bool Input::Mouse::onScroll()
{
	return pvt.onScrolled;
}

uint64_t Input::Mouse::addMoveEvent(const std::function<void(void)>& func)
{
	return pvt.moveEvent += func;
}

uint64_t Input::Mouse::addLeftDownEvent(const std::function<void(void)>& func)
{
	return pvt.leftDownEvent += func;
}

uint64_t Input::Mouse::addRightDownEvent(const std::function<void(void)>& func)
{
	return pvt.rightDownEvent += func;
}

uint64_t Input::Mouse::addLeftUpEvent(const std::function<void(void)>& func)
{
	return pvt.leftUpEvent += func;
}

uint64_t Input::Mouse::addRightUpEvent(const std::function<void(void)>& func)
{
	return pvt.rightUpEvent += func;
}

uint64_t Input::Mouse::addScrollEvent(const std::function<void(void)>& func)
{
	return pvt.scrollEvent += func;
}

void Input::Mouse::removeMoveEvent(const uint64_t id)
{
	pvt.moveEvent -= id;
}

void Input::Mouse::removeLeftDownEvent(const uint64_t id)
{
	pvt.leftDownEvent -= id;
}

void Input::Mouse::removeRightDownEvent(const uint64_t id)
{
	pvt.rightDownEvent -= id;
}

void Input::Mouse::removeLeftUpEvent(const uint64_t id)
{
	pvt.leftUpEvent -= id;
}

void Input::Mouse::removeRightUpEvent(const uint64_t id)
{
	pvt.rightUpEvent -= id;
}

void Input::Mouse::removeScrollEvent(const uint64_t id)
{
	pvt.scrollEvent -= id;
}

void Input::Mouse::Internal::resetDeltaValue()
{
	pvt.dx = 0;

	pvt.dy = 0;

	pvt.onMoved = false;

	pvt.onLeftDowned = false;

	pvt.onRightDowned = false;

	pvt.onScrolled = false;
}

void Input::Mouse::Internal::pressLeft()
{
	pvt.leftDown = true;

	pvt.onLeftDowned = true;

	pvt.leftDownEvent();
}

void Input::Mouse::Internal::pressRight()
{
	pvt.rightDown = true;

	pvt.onRightDowned = true;

	pvt.rightDownEvent();
}

void Input::Mouse::Internal::releaseLeft()
{
	pvt.leftDown = false;

	pvt.leftUpEvent();
}

void Input::Mouse::Internal::releaseRight()
{
	pvt.rightDown = false;

	pvt.rightUpEvent();
}

void Input::Mouse::Internal::scroll(const float delta)
{
	pvt.wheelDelta = delta;

	pvt.onScrolled = true;

	pvt.scrollEvent();
}

void Input::Mouse::Internal::move(const float curX, const float curY)
{
	pvt.dx = curX - pvt.x;

	pvt.dy = curY - pvt.y;

	pvt.x = curX;

	pvt.y = curY;

	pvt.onMoved = true;

	pvt.moveEvent();
}

