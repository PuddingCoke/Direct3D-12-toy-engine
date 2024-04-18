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

	static float getX();

	static float getY();

	static float getDX();

	static float getDY();

	static float getWheelDelta();

	static bool getLeftDown();

	static bool getRightDown();

	static bool onMove();

	static bool onLeftDown();

	static bool onRightDown();

	static bool onScroll();

	//do not record command here
	static int addMoveEvent(const std::function<void(void)>& func);

	//do not record command here
	static int addLeftDownEvent(const std::function<void(void)>& func);

	//do not record command here
	static int addRightDownEvent(const std::function<void(void)>& func);

	//do not record command here
	static int addLeftUpEvent(const std::function<void(void)>& func);

	//do not record command here
	static int addRightUpEvent(const std::function<void(void)>& func);

	//do not record command here
	static int addScrollEvent(const std::function<void(void)>& func);

	static void removeMoveEvent(const int id);

	static void removeLeftDownEvent(const int id);

	static void removeRightDownEvent(const int id);

	static void removeLeftUpEvent(const int id);

	static void removeRightUpEvent(const int id);

	static void removeScrollEvent(const int id);

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
