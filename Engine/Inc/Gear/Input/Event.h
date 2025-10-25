#pragma once

#ifndef _INPUT_EVENT_H_
#define _INPUT_EVENT_H_

#include<map>
#include<functional>
#include<cstdint>

namespace Input
{
	class Event
	{
	public:

		Event();

		void operator-=(const uint64_t id);

		uint64_t operator+=(const std::function<void(void)>& func);

		void operator()();

	private:

		uint64_t idx;

		std::map<uint64_t, std::function<void(void)>> functions;

	};
}

#endif // !_INPUT_EVENT_H_