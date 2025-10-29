#include<Gear/Input/Keyboard.h>

#include<Gear/Input/Internal/KeyboardInternal.h>

namespace
{
	struct KeyboardPrivate
	{

		static constexpr size_t maxKey = 512;

		Gear::Input::Event keyDownEvents[maxKey] = {};

		Gear::Input::Event keyUpEvents[maxKey] = {};

		bool keyDownStates[maxKey] = {};

		bool onKeyDownStates[maxKey] = {};

		std::vector<uint32_t> onKeyDownClearList = std::vector<uint32_t>();

	} pvt;
}

bool Gear::Input::Keyboard::getKeyDown(const Key key)
{
	return pvt.keyDownStates[key];
}

bool Gear::Input::Keyboard::onKeyDown(const Key key)
{
	return pvt.onKeyDownStates[key];
}

uint64_t Gear::Input::Keyboard::addKeyDownEvent(const Key key, const std::function<void(void)>& func)
{
	return pvt.keyDownEvents[key] += func;
}

uint64_t Gear::Input::Keyboard::addKeyUpEvent(const Key key, const std::function<void(void)>& func)
{
	return pvt.keyUpEvents[key] += func;
}

void Gear::Input::Keyboard::removeKeyDownEvent(const Key key, const uint64_t id)
{
	pvt.keyDownEvents[key] -= id;
}

void Gear::Input::Keyboard::removeKeyUpEvent(const Key key, const uint64_t id)
{
	pvt.keyUpEvents[key] -= id;
}

void Gear::Input::Keyboard::Internal::resetDeltaValue()
{
	if (pvt.onKeyDownClearList.size())
	{
		for (uint32_t i = 0; i < pvt.onKeyDownClearList.size(); i++)
		{
			const uint32_t idx = pvt.onKeyDownClearList[i];

			pvt.onKeyDownStates[idx] = false;
		}

		pvt.onKeyDownClearList.clear();
	}
}

void Gear::Input::Keyboard::Internal::pressKey(const Key key)
{
	pvt.keyDownStates[key] = true;

	pvt.onKeyDownStates[key] = true;

	pvt.onKeyDownClearList.emplace_back(key);

	pvt.keyDownEvents[key]();
}

void Gear::Input::Keyboard::Internal::releaseKey(const Key key)
{
	pvt.keyDownStates[key] = false;

	pvt.keyUpEvents[key]();
}
