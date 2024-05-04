#include<Gear/Utils/DeltaTimeEstimator.h>

DeltaTimeEstimator::DeltaTimeEstimator() :
	historyDeltatime{}, historyDeltaTimeIndex(0), populated(false)
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

	float sortedArray[11] = {};

	memcpy(sortedArray, historyDeltatime, sizeof(float) * 11);

	std::sort(sortedArray, sortedArray + 11);

	float averageDeltaTime = 0.f;

	for (unsigned int i = 2; i < 9; i++)
	{
		averageDeltaTime += sortedArray[i];
	}

	averageDeltaTime /= 7.f;

	const float lerpDeltaTime = averageDeltaTime * (1.f - lerpFactor) + lastDeltaTime * lerpFactor;

	return lerpDeltaTime;
}
