#pragma once

#ifndef _DELTATIMEESTIMATOR_H_
#define _DELTATIMEESTIMATOR_H_

#include<algorithm>

class DeltaTimeEstimator
{
public:

	DeltaTimeEstimator();

	~DeltaTimeEstimator();

	float getDeltaTime(const float lastDeltaTime);

private:

	static constexpr float lerpFactor = 0.1f;

	float historyDeltatime[11];

	unsigned int historyDeltaTimeIndex;

	bool populated;

};

#endif // !_DELTATIMEESTIMATOR_H_
