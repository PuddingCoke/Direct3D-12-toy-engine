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

		bool onMove = false;

		bool onLeftDown = false;

		bool onRightDown = false;

		bool onLeftUp = false;

		bool onRightUp = false;

		bool onScroll = false;

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
			impl.deltaX = 0.f;

			impl.deltaY = 0.f;

			impl.wheelDelta = 0.f;

			impl.onMove = false;

			impl.onLeftDown = false;

			impl.onRightDown = false;

			impl.onLeftUp = false;

			impl.onRightUp = false;

			impl.onScroll = false;
		}

		void pressLeft()
		{
			impl.leftDown = true;

			impl.onLeftDown = true;
		}

		void pressRight()
		{
			impl.rightDown = true;

			impl.onRightDown = true;
		}

		void releaseLeft()
		{
			impl.leftDown = false;

			impl.onLeftUp = true;
		}

		void releaseRight()
		{
			impl.rightDown = false;

			impl.onRightUp = true;
		}

		void scroll(const float delta)
		{
			impl.wheelDelta += delta;

			impl.onScroll = true;
		}

		void setPosition(const float x, const float y)
		{
			impl.x = x;

			impl.y = y;
		}

		void move(const float deltaX, const float deltaY)
		{
			impl.deltaX += deltaX;

			impl.deltaY += deltaY;

			impl.onMove = true;
		}

		void triggerEvents()
		{
			if (impl.onMove)
			{
				impl.moveEvent();
			}

			if (impl.onScroll)
			{
				impl.scrollEvent();
			}

			if (impl.onLeftDown)
			{
				impl.leftDownEvent();
			}

			if (impl.onLeftUp)
			{
				impl.leftUpEvent();
			}

			if (impl.onRightDown)
			{
				impl.rightDownEvent();
			}

			if (impl.onRightUp)
			{
				impl.rightUpEvent();
			}
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

	bool getOnMove()
	{
		return impl.onMove;
	}

	bool getOnLeftDown()
	{
		return impl.onLeftDown;
	}

	bool getOnRightDown()
	{
		return impl.onRightDown;
	}

	bool getOnLeftUp()
	{
		return impl.onLeftUp;
	}

	bool getOnRightUp()
	{
		return impl.onRightUp;
	}

	bool getOnScroll()
	{
		return impl.onScroll;
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

