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
	static uint64_t addMoveEvent(const std::function<void(void)>& func);

	//do not record command here
	static uint64_t addLeftDownEvent(const std::function<void(void)>& func);

	//do not record command here
	static uint64_t addRightDownEvent(const std::function<void(void)>& func);

	//do not record command here
	static uint64_t addLeftUpEvent(const std::function<void(void)>& func);

	//do not record command here
	static uint64_t addRightUpEvent(const std::function<void(void)>& func);

	//do not record command here
	static uint64_t addScrollEvent(const std::function<void(void)>& func);

	static void removeMoveEvent(const uint64_t id);

	static void removeLeftDownEvent(const uint64_t id);

	static void removeRightDownEvent(const uint64_t id);

	static void removeLeftUpEvent(const uint64_t id);

	static void removeRightUpEvent(const uint64_t id);

	static void removeScrollEvent(const uint64_t id);

private:

	friend class Win32Form;

	static void resetDeltaValue();

	static void pressLeft();

	static void pressRight();

	static void releaseLeft();

	static void releaseRight();

	static void scroll(const float delta);

	static void move(const float curX, const float curY);

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