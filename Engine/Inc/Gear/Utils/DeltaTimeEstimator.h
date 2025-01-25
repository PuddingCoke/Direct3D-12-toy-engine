#pragma once

#ifndef _DELTATIMEESTIMATOR_H_
#define _DELTATIMEESTIMATOR_H_

#include<cstdint>

class DeltaTimeEstimator
{
public:

	DeltaTimeEstimator(const DeltaTimeEstimator&) = delete;

	void operator=(const DeltaTimeEstimator&) = delete;

	DeltaTimeEstimator();

	~DeltaTimeEstimator();

	float getDeltaTime(const float lastDeltaTime);

private:

	static constexpr float lerpFactor = 0.065f;

	float historyDeltatime[11];

	float sortedDeltaTime[11];

	uint32_t historyDeltaTimeIndex;

	bool populated;

};

#endif // !_DELTATIMEESTIMATOR_H_