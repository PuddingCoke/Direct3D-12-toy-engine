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

	static constexpr uint32_t numRecord = 11;

	static constexpr uint32_t numDiscard = 2;

	float historyDeltatime[numRecord];

	float sortedDeltaTime[numRecord];

	uint32_t historyDeltaTimeIndex;

	bool populated;

};

#endif // !_DELTATIMEESTIMATOR_H_