#include<Gear/Gear.h>

#include"MyGame.h"

int wmain(int argc, const wchar_t* argv[])
{
	const InitializationParam param = InitializationParam::VideoRender(1920, 1080);

	Gear::initialize();

	if (!Gear::iniEngine(param, argc, argv))
	{
		Gear::iniGame(new MyGame());
	}

	Gear::release();
}