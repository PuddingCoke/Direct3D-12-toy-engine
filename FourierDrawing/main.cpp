#include<Gear/Gear.h>

#include"MyGame.h"

using namespace Gear;

int wmain(int argc, const wchar_t* argv[])
{
	const InitializationParam param = InitializationParam::RealTime(1000, 1000, L"Fourier Drawing");

	Gear::initialize();

	if (!Gear::iniEngine(param, argc, argv))
	{
		Gear::iniGame(new MyGame());
	}

	Gear::release();
}