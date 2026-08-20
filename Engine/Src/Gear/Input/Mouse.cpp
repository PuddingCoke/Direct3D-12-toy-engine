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

		Event moveEvent;

		Event leftDownEvent;

		Event rightDownEvent;

		Event leftUpEvent;

		Event rightUpEvent;

		Event scrollEvent;

		std::vector<Event*> mouseEventList = std::vector<Event*>();

	} impl;

	namespace Internal
	{
		void resetDeltaValue()
		{
			impl.deltaX = 0.f;

			impl.deltaY = 0.f;

			impl.wheelDelta = 0.f;

			if (impl.mouseEventList.size())
			{
				for (auto& eventPtr : impl.mouseEventList)
				{
					eventPtr->resetTriggerCount();
				}

				impl.mouseEventList.clear();
			}
		}

		void pressLeft()
		{
			if (!getOnLeftDown())
			{
				impl.mouseEventList.emplace_back(&impl.leftDownEvent);
			}

			impl.leftDown = true;

			impl.leftDownEvent.increaseTriggerCount();
		}

		void pressRight()
		{
			if (!getOnRightDown())
			{
				impl.mouseEventList.emplace_back(&impl.rightDownEvent);
			}

			impl.rightDown = true;

			impl.rightDownEvent.increaseTriggerCount();
		}

		void releaseLeft()
		{
			if (!getOnLeftUp())
			{
				impl.mouseEventList.emplace_back(&impl.leftUpEvent);
			}

			impl.leftDown = false;

			impl.leftUpEvent.increaseTriggerCount();
		}

		void releaseRight()
		{
			if (!getOnRightUp())
			{
				impl.mouseEventList.emplace_back(&impl.rightUpEvent);
			}

			impl.rightDown = false;

			impl.rightUpEvent.increaseTriggerCount();
		}

		void scroll(const float delta)
		{
			if (!getOnScroll())
			{
				impl.mouseEventList.emplace_back(&impl.scrollEvent);
			}

			impl.wheelDelta += delta;

			impl.scrollEvent.increaseTriggerCount();
		}

		void setPosition(const float x, const float y)
		{
			impl.x = x;

			impl.y = y;
		}

		void move(const float deltaX, const float deltaY)
		{
			if (!getOnMove())
			{
				impl.mouseEventList.emplace_back(&impl.moveEvent);
			}

			impl.deltaX += deltaX;

			impl.deltaY += deltaY;

			impl.moveEvent.increaseTriggerCount();
		}

		void triggerEvents()
		{
			if (impl.mouseEventList.size())
			{
				for (auto& eventPtr : impl.mouseEventList)
				{
					eventPtr->trigger();
				}
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

	uint32_t getMoveTriggerCount()
	{
		return impl.moveEvent.getTriggerCount();
	}

	uint32_t getLeftDownTriggerCount()
	{
		return impl.leftDownEvent.getTriggerCount();
	}

	uint32_t getRightDownTriggerCount()
	{
		return impl.rightDownEvent.getTriggerCount();
	}

	uint32_t getLeftUpTriggerCount()
	{
		return impl.leftUpEvent.getTriggerCount();
	}

	uint32_t getRightUpTriggerCount()
	{
		return impl.rightUpEvent.getTriggerCount();
	}

	uint32_t getScrollTriggerCount()
	{
		return impl.scrollEvent.getTriggerCount();
	}

	bool getOnMove()
	{
		return getMoveTriggerCount();
	}

	bool getOnLeftDown()
	{
		return getLeftDownTriggerCount();
	}

	bool getOnRightDown()
	{
		return getRightDownTriggerCount();
	}

	bool getOnLeftUp()
	{
		return getLeftUpTriggerCount();
	}

	bool getOnRightUp()
	{
		return getRightUpTriggerCount();
	}

	bool getOnScroll()
	{
		return getScrollTriggerCount();
	}

	uint64_t addMoveEvent(const std::function<void(const uint32_t)>& func)
	{
		return impl.moveEvent += func;
	}

	uint64_t addLeftDownEvent(const std::function<void(const uint32_t)>& func)
	{
		return impl.leftDownEvent += func;
	}

	uint64_t addRightDownEvent(const std::function<void(const uint32_t)>& func)
	{
		return impl.rightDownEvent += func;
	}

	uint64_t addLeftUpEvent(const std::function<void(const uint32_t)>& func)
	{
		return impl.leftUpEvent += func;
	}

	uint64_t addRightUpEvent(const std::function<void(const uint32_t)>& func)
	{
		return impl.rightUpEvent += func;
	}

	uint64_t addScrollEvent(const std::function<void(const uint32_t)>& func)
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
