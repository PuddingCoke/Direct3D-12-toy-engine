#include<Gear/Input/Keyboard.h>

Event Keyboard::keyDownEvents[Keyboard::maxKey] = {};

Event Keyboard::keyUpEvents[Keyboard::maxKey] = {};

bool Keyboard::keyDownStates[Keyboard::maxKey] = {};

bool Keyboard::onKeyDownStates[Keyboard::maxKey] = {};

std::vector<uint32_t> Keyboard::onKeyDownClearList = std::vector<uint32_t>();

bool Keyboard::getKeyDown(const Key key)
{
	return keyDownStates[key];
}

bool Keyboard::onKeyDown(const Key key)
{
	return onKeyDownStates[key];
}

uint64_t Keyboard::addKeyDownEvent(const Key key, const std::function<void(void)>& func)
{
	return keyDownEvents[key] += func;
}

uint64_t Keyboard::addKeyUpEvent(const Key key, const std::function<void(void)>& func)
{
	return keyUpEvents[key] += func;
}

void Keyboard::removeKeyDownEvent(const Key key, const uint64_t id)
{
	keyDownEvents[key] -= id;
}

void Keyboard::removeKeyUpEvent(const Key key, const uint64_t id)
{
	keyUpEvents[key] -= id;
}

void Keyboard::resetDeltaValue()
{
	if (onKeyDownClearList.size())
	{
		for (uint32_t i = 0; i < onKeyDownClearList.size(); i++)
		{
			const uint32_t idx = onKeyDownClearList[i];

			onKeyDownStates[idx] = false;
		}

		onKeyDownClearList.clear();
	}
}

void Keyboard::pressKey(const Key key)
{
	keyDownStates[key] = true;

	onKeyDownStates[key] = true;

	onKeyDownClearList.emplace_back(key);

	keyDownEvents[key]();
}

void Keyboard::releaseKey(const Key key)
{
	keyDownStates[key] = false;

	keyUpEvents[key]();
}
