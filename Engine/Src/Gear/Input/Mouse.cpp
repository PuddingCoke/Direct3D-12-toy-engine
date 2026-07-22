#include<Gear/Input/Mouse.h>

#include<Gear/Input/Internal/MouseInternal.h>

#include<Gear/Input/Event.h>

namespace Gear::Input::Mouse
{
	struct MouseImpl
	{

		float x = 0.f;

		float y = 0.f;

		float deltaX = 0.f;

		float deltaY = 0.f;

		float wheelDelta = 0.f;

		bool leftDown = false;

		bool rightDown = false;

		bool onMoved = false;

		bool onLeftDowned = false;

		bool onRightDowned = false;

		bool onScrolled = false;

		Event moveEvent;

		Event leftDownEvent;

		Event rightDownEvent;

		Event leftUpEvent;

		Event rightUpEvent;

		Event scrollEvent;

	}impl;

	namespace Internal
	{
		void resetDeltaValue()
		{
			impl.deltaX = 0;

			impl.deltaY = 0;

			impl.onMoved = false;

			impl.onLeftDowned = false;

			impl.onRightDowned = false;

			impl.onScrolled = false;
		}

		void pressLeft()
		{
			impl.leftDown = true;

			impl.onLeftDowned = true;

			impl.leftDownEvent();
		}

		void pressRight()
		{
			impl.rightDown = true;

			impl.onRightDowned = true;

			impl.rightDownEvent();
		}

		void releaseLeft()
		{
			impl.leftDown = false;

			impl.leftUpEvent();
		}

		void releaseRight()
		{
			impl.rightDown = false;

			impl.rightUpEvent();
		}

		void scroll(const float delta)
		{
			impl.wheelDelta = delta;

			impl.onScrolled = true;

			impl.scrollEvent();
		}

		void move(const float curX, const float curY)
		{
			impl.deltaX = curX - impl.x;

			impl.deltaY = curY - impl.y;

			impl.x = curX;

			impl.y = curY;

			impl.onMoved = true;

			impl.moveEvent();
		}
	}

	float getX()
	{
		return impl.x;
	}

	float getY()
	{
		return impl.y;
	}

	float getDeltaX()
	{
		return impl.deltaX;
	}

	float getDeltaY()
	{
		return impl.deltaY;
	}

	float getWheelDelta()
	{
		return impl.wheelDelta;
	}

	bool getLeftDown()
	{
		return impl.leftDown;
	}

	bool getRightDown()
	{
		return impl.rightDown;
	}

	bool onMove()
	{
		return impl.onMoved;
	}

	bool onLeftDown()
	{
		return impl.onLeftDowned;
	}

	bool onRightDown()
	{
		return impl.onRightDowned;
	}

	bool onScroll()
	{
		return impl.onScrolled;
	}

	uint64_t addMoveEvent(const std::function<void(void)>& func)
	{
		return impl.moveEvent += func;
	}

	uint64_t addLeftDownEvent(const std::function<void(void)>& func)
	{
		return impl.leftDownEvent += func;
	}

	uint64_t addRightDownEvent(const std::function<void(void)>& func)
	{
		return impl.rightDownEvent += func;
	}

	uint64_t addLeftUpEvent(const std::function<void(void)>& func)
	{
		return impl.leftUpEvent += func;
	}

	uint64_t addRightUpEvent(const std::function<void(void)>& func)
	{
		return impl.rightUpEvent += func;
	}

	uint64_t addScrollEvent(const std::function<void(void)>& func)
	{
		return impl.scrollEvent += func;
	}

	void removeMoveEvent(const uint64_t id)
	{
		impl.moveEvent -= id;
	}

	void removeLeftDownEvent(const uint64_t id)
	{
		impl.leftDownEvent -= id;
	}

	void removeRightDownEvent(const uint64_t id)
	{
		impl.rightDownEvent -= id;
	}

	void removeLeftUpEvent(const uint64_t id)
	{
		impl.leftUpEvent -= id;
	}

	void removeRightUpEvent(const uint64_t id)
	{
		impl.rightUpEvent -= id;
	}

	void removeScrollEvent(const uint64_t id)
	{
		impl.scrollEvent -= id;
	}
}

