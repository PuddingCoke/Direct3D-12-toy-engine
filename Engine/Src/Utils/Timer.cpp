#include<Gear/Utils/Timer.h>

Utils::Timer::Timer() :
	curTime(0), timeLimit(0)
{

}

Utils::Timer::Timer(const float timeLimit) :
	timeLimit(timeLimit), curTime(0)
{

}

bool Utils::Timer::update(const float dt)
{
	if (curTime >= timeLimit)
	{
		curTime -= timeLimit;

		return true;
	}

	curTime += dt;

	return false;
}

void Utils::Timer::restart()
{
	curTime = 0;
}

void Utils::Timer::setTimeLimit(const float timeLimit)
{
	this->timeLimit = timeLimit;
}

float Utils::Timer::getTimeLimit() const
{
	return timeLimit;
}
