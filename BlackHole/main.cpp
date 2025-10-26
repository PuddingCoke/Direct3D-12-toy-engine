#include<Gear/Gear.h>

#include"MyGame.h"

int wmain(int argc, const wchar_t* argv[])
{
	const InitializationParam param = InitializationParam::RealTime(1920, 1080, L"Black Hole", true);

	//const InitializationParam param = InitializationParam::VideoRender(2560, 1440, 360);

	Gear::initialize();

	if (!Gear::iniEngine(param, argc, argv))
	{
		Gear::iniGame(new MyGame());
	}

	Gear::release();
}