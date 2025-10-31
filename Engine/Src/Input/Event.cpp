#include<Gear/Input/Event.h>

Gear::Input::Event::Event() :
	idx(0)
{
}

void Gear::Input::Event::operator-=(const uint64_t id)
{
	{
		std::lock_guard<std::mutex> lockGuard(containerMutex);

		functions.erase(id);
	}
}

uint64_t Gear::Input::Event::operator+=(const std::function<void(void)>& func)
{
	uint64_t retIndex;

	{
		std::lock_guard<std::mutex> lockGuard(containerMutex);

		retIndex = idx++;

		functions.emplace(retIndex, func);
	}

	return retIndex;
}

void Gear::Input::Event::operator()()
{
	for (auto& i : functions)
	{
		i.second();
	}
}