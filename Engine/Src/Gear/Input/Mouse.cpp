﻿#include<Gear/Input/Mouse.h>

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

int Mouse::addMoveEvent(const std::function<void(void)>& func)
{
	return moveEvent += func;
}

int Mouse::addLeftDownEvent(const std::function<void(void)>& func)
{
	return leftDownEvent += func;
}

int Mouse::addRightDownEvent(const std::function<void(void)>& func)
{
	return rightDownEvent += func;
}

int Mouse::addLeftUpEvent(const std::function<void(void)>& func)
{
	return leftUpEvent += func;
}

int Mouse::addRightUpEvent(const std::function<void(void)>& func)
{
	return rightUpEvent += func;
}

int Mouse::addScrollEvent(const std::function<void(void)>& func)
{
	return scrollEvent += func;
}

void Mouse::removeMoveEvent(const int id)
{
	moveEvent -= id;
}

void Mouse::removeLeftDownEvent(const int id)
{
	leftDownEvent -= id;
}

void Mouse::removeRightDownEvent(const int id)
{
	rightDownEvent -= id;
}

void Mouse::removeLeftUpEvent(const int id)
{
	leftUpEvent -= id;
}

void Mouse::removeRightUpEvent(const int id)
{
	rightUpEvent -= id;
}

void Mouse::removeScrollEvent(const int id)
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
