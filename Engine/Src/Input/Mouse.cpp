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

		Gear::Input::Event moveEvent;

		Gear::Input::Event leftDownEvent;

		Gear::Input::Event rightDownEvent;

		Gear::Input::Event leftUpEvent;

		Gear::Input::Event rightUpEvent;

		Gear::Input::Event scrollEvent;

	}pvt;
}

float Gear::Input::Mouse::getX()
{
	return pvt.x;
}

float Gear::Input::Mouse::getY()
{
	return pvt.y;
}

float Gear::Input::Mouse::getDX()
{
	return pvt.dx;
}

float Gear::Input::Mouse::getDY()
{
	return pvt.dy;
}

float Gear::Input::Mouse::getWheelDelta()
{
	return pvt.wheelDelta;
}

bool Gear::Input::Mouse::getLeftDown()
{
	return pvt.leftDown;
}

bool Gear::Input::Mouse::getRightDown()
{
	return pvt.rightDown;
}

bool Gear::Input::Mouse::onMove()
{
	return pvt.onMoved;
}

bool Gear::Input::Mouse::onLeftDown()
{
	return pvt.onLeftDowned;
}

bool Gear::Input::Mouse::onRightDown()
{
	return pvt.onRightDowned;
}

bool Gear::Input::Mouse::onScroll()
{
	return pvt.onScrolled;
}

uint64_t Gear::Input::Mouse::addMoveEvent(const std::function<void(void)>& func)
{
	return pvt.moveEvent += func;
}

uint64_t Gear::Input::Mouse::addLeftDownEvent(const std::function<void(void)>& func)
{
	return pvt.leftDownEvent += func;
}

uint64_t Gear::Input::Mouse::addRightDownEvent(const std::function<void(void)>& func)
{
	return pvt.rightDownEvent += func;
}

uint64_t Gear::Input::Mouse::addLeftUpEvent(const std::function<void(void)>& func)
{
	return pvt.leftUpEvent += func;
}

uint64_t Gear::Input::Mouse::addRightUpEvent(const std::function<void(void)>& func)
{
	return pvt.rightUpEvent += func;
}

uint64_t Gear::Input::Mouse::addScrollEvent(const std::function<void(void)>& func)
{
	return pvt.scrollEvent += func;
}

void Gear::Input::Mouse::removeMoveEvent(const uint64_t id)
{
	pvt.moveEvent -= id;
}

void Gear::Input::Mouse::removeLeftDownEvent(const uint64_t id)
{
	pvt.leftDownEvent -= id;
}

void Gear::Input::Mouse::removeRightDownEvent(const uint64_t id)
{
	pvt.rightDownEvent -= id;
}

void Gear::Input::Mouse::removeLeftUpEvent(const uint64_t id)
{
	pvt.leftUpEvent -= id;
}

void Gear::Input::Mouse::removeRightUpEvent(const uint64_t id)
{
	pvt.rightUpEvent -= id;
}

void Gear::Input::Mouse::removeScrollEvent(const uint64_t id)
{
	pvt.scrollEvent -= id;
}

void Gear::Input::Mouse::Internal::resetDeltaValue()
{
	pvt.dx = 0;

	pvt.dy = 0;

	pvt.onMoved = false;

	pvt.onLeftDowned = false;

	pvt.onRightDowned = false;

	pvt.onScrolled = false;
}

void Gear::Input::Mouse::Internal::pressLeft()
{
	pvt.leftDown = true;

	pvt.onLeftDowned = true;

	pvt.leftDownEvent();
}

void Gear::Input::Mouse::Internal::pressRight()
{
	pvt.rightDown = true;

	pvt.onRightDowned = true;

	pvt.rightDownEvent();
}

void Gear::Input::Mouse::Internal::releaseLeft()
{
	pvt.leftDown = false;

	pvt.leftUpEvent();
}

void Gear::Input::Mouse::Internal::releaseRight()
{
	pvt.rightDown = false;

	pvt.rightUpEvent();
}

void Gear::Input::Mouse::Internal::scroll(const float delta)
{
	pvt.wheelDelta = delta;

	pvt.onScrolled = true;

	pvt.scrollEvent();
}

void Gear::Input::Mouse::Internal::move(const float curX, const float curY)
{
	pvt.dx = curX - pvt.x;

	pvt.dy = curY - pvt.y;

	pvt.x = curX;

	pvt.y = curY;

	pvt.onMoved = true;

	pvt.moveEvent();
}

