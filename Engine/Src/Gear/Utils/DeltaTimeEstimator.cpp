#include<Gear/Utils/DeltaTimeEstimator.h>

DeltaTimeEstimator::DeltaTimeEstimator() :
	historyDeltatime{}, sortedDeltaTime{}, historyDeltaTimeIndex(0), populated(false)
{
}

DeltaTimeEstimator::~DeltaTimeEstimator()
{
}

float DeltaTimeEstimator::getDeltaTime(const float lastDeltaTime)
{
	historyDeltatime[historyDeltaTimeIndex] = lastDeltaTime;

	if (historyDeltaTimeIndex == 10)
	{
		populated = true;
	}

	historyDeltaTimeIndex = (historyDeltaTimeIndex + 1) % 11;

	if (!populated)
	{
		return lastDeltaTime;
	}

	memcpy(sortedDeltaTime, historyDeltatime, sizeof(float) * 11);

	std::sort(sortedDeltaTime, sortedDeltaTime + 11);

	float averageDeltaTime = 0.f;

	for (unsigned int i = 2; i < 9; i++)
	{
		averageDeltaTime += sortedDeltaTime[i];
	}

	averageDeltaTime /= 7.f;

	const float lerpDeltaTime = averageDeltaTime * (1.f - lerpFactor) + lastDeltaTime * lerpFactor;

	return lerpDeltaTime;
}
