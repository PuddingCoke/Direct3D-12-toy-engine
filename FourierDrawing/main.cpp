#include<Gear/Gear.h>

#include"MyGame.h"

using EngineUsage = Configuration::EngineUsage;

int wmain(int argc, const wchar_t* argv[])
{
	Configuration config(1000, 1000, L"Fourier Drawing", EngineUsage::NORMAL);

	Gear::initialize();

	if (!Gear::get()->iniEngine(config, argc, argv))
	{
		Gear::get()->iniGame(new MyGame());
	}

	Gear::release();
}