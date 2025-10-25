#include<Gear/Input/Event.h>

Input::Event::Event() :
	idx(0)
{
}

void Input::Event::operator-=(const uint64_t id)
{
	functions.erase(id);
}

uint64_t Input::Event::operator+=(const std::function<void(void)>& func)
{
	const uint64_t retIdx = idx++;

	functions.emplace(retIdx, func);

	return retIdx;
}

void Input::Event::operator()()
{
	for (auto& i : functions)
	{
		i.second();
	}
}