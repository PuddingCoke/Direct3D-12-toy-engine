#include<Gear/Gear.h>

#include"MyGame.h"

using EngineUsage = Configuration::EngineUsage;

int main(int argc, const char* argv[])
{
	Configuration config(1920, 1080, L"Crytek Sponza", EngineUsage::NORMAL, true);

	Gear::initialize();

	if (!Gear::get()->iniEngine(config, argc, argv))
	{
		Gear::get()->iniGame(new MyGame());
	}

	Gear::release();
}