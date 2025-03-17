#include<Gear/Input/Mouse.h>

float Mouse::x = 0;
float Mouse::y = 0;

float Mouse::dx = 0;
float Mouse::dy = 0;

float Mouse::wheelDelta = 0;

Event Mouse::moveEvent;
Event Mouse::leftDownEvent;
Event Mouse::rightDownEvent;
Event Mouse::leftUpEvent;
Event Mouse::rightUpEvent;
Event Mouse::scrollEvent;

bool Mouse::leftDown = false;
bool Mouse::rightDown = false;

bool Mouse::onMoved = false;
bool Mouse::onLeftDowned = false;
bool Mouse::onRightDowned = false;
bool Mouse::onScrolled = false;

float Mouse::getX()
{
	return x;
}

float Mouse::getY()
{
	return y;
}

float Mouse::getDX()
{
	return dx;
}

float Mouse::getDY()
{
	return dy;
}

float Mouse::getWheelDelta()
{
	return wheelDelta;
}

bool Mouse::getLeftDown()
{
	return leftDown;
}

bool Mouse::getRightDown()
{
	return rightDown;
}

bool Mouse::onMove()
{
	return onMoved;
}

bool Mouse::onLeftDown()
{
	return onLeftDowned;
}

bool Mouse::onRightDown()
{
	return onRightDowned;
}

bool Mouse::onScroll()
{
	return onScrolled;
}

uint64_t Mouse::addMoveEvent(const std::function<void(void)>& func)
{
	return moveEvent += func;
}

uint64_t Mouse::addLeftDownEvent(const std::function<void(void)>& func)
{
	return leftDownEvent += func;
}

uint64_t Mouse::addRightDownEvent(const std::function<void(void)>& func)
{
	return rightDownEvent += func;
}

uint64_t Mouse::addLeftUpEvent(const std::function<void(void)>& func)
{
	return leftUpEvent += func;
}

uint64_t Mouse::addRightUpEvent(const std::function<void(void)>& func)
{
	return rightUpEvent += func;
}

uint64_t Mouse::addScrollEvent(const std::function<void(void)>& func)
{
	return scrollEvent += func;
}

void Mouse::removeMoveEvent(const uint64_t id)
{
	moveEvent -= id;
}

void Mouse::removeLeftDownEvent(const uint64_t id)
{
	leftDownEvent -= id;
}

void Mouse::removeRightDownEvent(const uint64_t id)
{
	rightDownEvent -= id;
}

void Mouse::removeLeftUpEvent(const uint64_t id)
{
	leftUpEvent -= id;
}

void Mouse::removeRightUpEvent(const uint64_t id)
{
	rightUpEvent -= id;
}

void Mouse::removeScrollEvent(const uint64_t id)
{
	scrollEvent -= id;
}

void Mouse::resetDeltaInfo()
{
	dx = 0;

	dy = 0;

	onMoved = false;

	onLeftDowned = false;

	onRightDowned = false;

	onScrolled = false;
}

void Mouse::pressLeft()
{
	leftDown = true;

	onLeftDowned = true;

	leftDownEvent();
}

void Mouse::pressRight()
{
	rightDown = true;

	onRightDowned = true;

	rightDownEvent();
}

void Mouse::releaseLeft()
{
	leftDown = false;

	leftUpEvent();
}

void Mouse::releaseRight()
{
	rightDown = false;

	rightUpEvent();
}

void Mouse::scroll(const float delta)
{
	wheelDelta = delta;

	onScrolled = true;

	scrollEvent();
}

void Mouse::move(const float curX, const float curY)
{
	dx = curX - Mouse::x;

	dy = curY - Mouse::y;

	x = curX;

	y = curY;

	onMoved = true;

	moveEvent();
}

