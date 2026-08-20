#include<Gear/Input/Keyboard.h>

#include<Gear/Input/Internal/KeyboardInternal.h>

#include<Gear/Input/Event.h>

namespace Gear::Input::Keyboard
{
	struct KeyboardImpl
	{

		static constexpr size_t maxKey = 512;

		Event keyDownEvents[maxKey] = {};

		Event keyUpEvents[maxKey] = {};

		bool keyDownStates[maxKey] = {};

		std::vector<Event*> keyEventList = std::vector<Event*>();

	} impl;

	namespace Internal
	{

		void resetDeltaValue()
		{
			if (impl.keyEventList.size())
			{
				for (auto& eventPtr : impl.keyEventList)
				{
					eventPtr->resetTriggerCount();
				}

				impl.keyEventList.clear();
			}
		}

		void pressKey(const Key key)
		{
			if (!onKeyDown(key))
			{
				impl.keyEventList.emplace_back(&impl.keyDownEvents[key]);
			}

			impl.keyDownStates[key] = true;

			impl.keyDownEvents[key].increaseTriggerCount();
		}

		void releaseKey(const Key key)
		{
			if (!onKeyUp(key))
			{
				impl.keyEventList.emplace_back(&impl.keyUpEvents[key]);
			}

			impl.keyDownStates[key] = false;

			impl.keyUpEvents[key].increaseTriggerCount();
		}

		void triggerEvents()
		{
			if (impl.keyEventList.size())
			{
				for (auto& eventPtr : impl.keyEventList)
				{
					eventPtr->trigger();
				}
			}
		}

	}

	bool getKeyDown(const Key key)
	{
		return impl.keyDownStates[key];
	}

	uint32_t getKeyDownTriggerCount(const Key key)
	{
		return impl.keyDownEvents[key].getTriggerCount();
	}

	uint32_t getKeyUpTriggerCount(const Key key)
	{
		return impl.keyUpEvents[key].getTriggerCount();
	}

	bool onKeyDown(const Key key)
	{
		return getKeyDownTriggerCount(key);
	}

	bool onKeyUp(const Key key)
	{
		return getKeyUpTriggerCount(key);
	}

	uint64_t addKeyDownEvent(const Key key, const std::function<void(const uint32_t)>& func)
	{
		return impl.keyDownEvents[key] += func;
	}

	uint64_t addKeyUpEvent(const Key key, const std::function<void(const uint32_t)>& func)
	{
		return impl.keyUpEvents[key] += func;
	}

	void removeKeyDownEvent(const Key key, const uint64_t id)
	{
		impl.keyDownEvents[key] -= id;
	}

	void removeKeyUpEvent(const Key key, const uint64_t id)
	{
		impl.keyUpEvents[key] -= id;
	}
}
