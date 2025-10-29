#include<Gear/Utils/Timer.h>

Gear::Utils::Timer::Timer() :
	curTime(0), timeLimit(0)
{

}

Gear::Utils::Timer::Timer(const float timeLimit) :
	timeLimit(timeLimit), curTime(0)
{

}

bool Gear::Utils::Timer::update(const float dt)
{
	if (curTime >= timeLimit)
	{
		curTime -= timeLimit;

		return true;
	}

	curTime += dt;

	return false;
}

void Gear::Utils::Timer::restart()
{
	curTime = 0;
}

void Gear::Utils::Timer::setTimeLimit(const float timeLimit)
{
	this->timeLimit = timeLimit;
}

float Gear::Utils::Timer::getTimeLimit() const
{
	return timeLimit;
}
