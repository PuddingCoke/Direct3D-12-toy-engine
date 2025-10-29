#include<Gear/Input/Event.h>

Gear::Input::Event::Event() :
	idx(0)
{
}

void Gear::Input::Event::operator-=(const uint64_t id)
{
	functions.erase(id);
}

uint64_t Gear::Input::Event::operator+=(const std::function<void(void)>& func)
{
	const uint64_t retIdx = idx++;

	functions.emplace(retIdx, func);

	return retIdx;
}

void Gear::Input::Event::operator()()
{
	for (auto& i : functions)
	{
		i.second();
	}
}