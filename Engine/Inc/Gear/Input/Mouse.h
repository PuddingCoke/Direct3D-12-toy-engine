#pragma once

#ifndef _MOUSE_H_
#define _MOUSE_H_

#include"Event.h"

class Mouse
{
public:

	Mouse() = delete;

	Mouse(const Mouse&) = delete;

	void operator=(const Mouse&) = delete;

	static const float& getX();

	static const float& getY();

	static const float& getDX();

	static const float& getDY();

	static const float& getWheelDelta();

	static const bool& getLeftDown();

	static const bool& getRightDown();

	static const bool& onMove();

	static const bool& onLeftDown();

	static const bool& onRightDown();

	static const bool& onScroll();

	//do not record command here
	static int addMoveEvent(std::function<void(void)> func);

	//do not record command here
	static int addLeftDownEvent(std::function<void(void)> func);

	//do not record command here
	static int addRightDownEvent(std::function<void(void)> func);

	//do not record command here
	static int addLeftUpEvent(std::function<void(void)> func);

	//do not record command here
	static int addRightUpEvent(std::function<void(void)> func);

	//do not record command here
	static int addScrollEvent(std::function<void(void)> func);

	static void removeMoveEvent(const int& id);

	static void removeLeftDownEvent(const int& id);

	static void removeRightDownEvent(const int& id);

	static void removeLeftUpEvent(const int& id);

	static void removeRightUpEvent(const int& id);

	static void removeScrollEvent(const int& id);

private:

	friend class Gear;

	static void resetDeltaInfo();

	static float x;
	
	static float y;

	static float dx;

	static float dy;

	static float wheelDelta;

	static bool leftDown;

	static bool rightDown;

	static bool onMoved;

	static bool onLeftDowned;

	static bool onRightDowned;

	static bool onScrolled;

	static Event moveEvent;

	static Event leftDownEvent;

	static Event rightDownEvent;

	static Event leftUpEvent;

	static Event rightUpEvent;

	static Event scrollEvent;

};

#endif // !_MOUSE_H_
