#include<Gear/Gear.h>

#include"MyGame.h"

int wmain(int argc, const wchar_t* argv[])
{
	const InitializationParam param = InitializationParam::RealTime(1920, 1080, L"Fluid Simulation", true);

	Gear::initialize();

	if (!Gear::get()->iniEngine(param, argc, argv))
	{
		Gear::get()->iniGame(new MyGame());
	}

	Gear::release();
}