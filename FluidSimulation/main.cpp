#include<Gear/Gear.h>

#include"MyGame.h"

int wmain(int argc, const wchar_t* argv[])
{
	const InitializationParam param = InitializationParam::RealTime(1920, 1080, L"Fluid Simulation", true);

	//const InitializationParam param = InitializationParam::Wallpaper();

	Gear::initialize();

	if (!Gear::iniEngine(param, argc, argv))
	{
		Gear::iniGame(new MyGame());
	}

	Gear::release();
}