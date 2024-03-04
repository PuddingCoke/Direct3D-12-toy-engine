#pragma once

#include<Gear/Core/RenderPass.h>

class MyRenderPass:public RenderPass
{
public:

	MyRenderPass()
	{
		
	}

	~MyRenderPass()
	{

	}

protected:

	void recordCommand() override
	{
		float clearValue[4] = { sinf(Graphics::getTimeElapsed()) * 0.5f + 0.5f,cosf(Graphics::getTimeElapsed()) * 0.5f + 0.5f,0.f,1.f };
		clearDefRenderTarget(clearValue);
	}

private:

};