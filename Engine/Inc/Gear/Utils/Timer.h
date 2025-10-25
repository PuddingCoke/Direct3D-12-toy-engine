#pragma once

#ifndef _UTILS_TIMER_H_
#define _UTILS_TIMER_H_

namespace Utils
{
	class Timer
	{
	public:

		Timer();

		Timer(const float timeLimit);

		bool update(const float dt);

		void restart();

		void setTimeLimit(const float timeLimit);

		float getTimeLimit() const;

	private:

		float timeLimit;

		float curTime;

	};
}

#endif // !_UTILS_TIMER_H_