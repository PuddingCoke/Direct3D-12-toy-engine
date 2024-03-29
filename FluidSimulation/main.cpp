#include<Gear/Gear.h>

#include"MyGame.h"

int main(int argc, const char* argv[])
{
	Configuration config(1920, 1080, L"Fluid Simulation", Configuration::EngineUsage::NORMAL);

	Gear::initialize();

	if (!Gear::get()->iniEngine(config, argc, argv))
	{
		Gear::get()->iniGame(new MyGame());
	}

	Gear::release();
}