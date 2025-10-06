#include<Gear/Gear.h>

#include"MyGame.h"

int wmain(int argc, const wchar_t* argv[])
{
	const InitializationParam param = InitializationParam::RealTime(1000, 1000, L"Fourier Drawing");

	Gear::initialize();

	if (!Gear::get()->iniEngine(param, argc, argv))
	{
		Gear::get()->iniGame(new MyGame());
	}

	Gear::release();
}