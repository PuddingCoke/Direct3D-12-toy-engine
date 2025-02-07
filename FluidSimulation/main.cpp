#include<Gear/Gear.h>

#include"MyGame.h"

int wmain(int argc, const wchar_t* argv[])
{
	Configuration config(1920, 1080, L"Fluid Simulation", Configuration::EngineUsage::NORMAL, true);

	Gear::initialize();

	if (!Gear::get()->iniEngine(config, argc, argv))
	{
		Gear::get()->iniGame(new MyGame());
	}

	Gear::release();
}