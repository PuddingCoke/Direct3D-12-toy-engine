#pragma once

#include<Gear/Core/RenderPass.h>

class MyRenderPass :public RenderPass
{
public:

	void recordCommand() override
	{
		context->setDefRenderTarget();

		const float clearValue[4] = {
			cosf(Graphics::getTimeElapsed()) * 0.5 + 0.5,
			sinf(Graphics::getTimeElapsed()) * 0.5 + 0.5,
			1.0,
			1.0
		};

		context->clearDefRenderTarget(clearValue);
	}

private:

};