#include<Gear/Input/Event.h>

Event::Event() :
	idx(0)
{
}

void Event::operator-=(const uint64_t id)
{
	functions.erase(id);
}

uint64_t Event::operator+=(const std::function<void(void)>& func)
{
	functions.emplace(++idx, func);

	return idx;
}

void Event::operator()()
{
	for (auto& i : functions)
	{
		i.second();
	}
}