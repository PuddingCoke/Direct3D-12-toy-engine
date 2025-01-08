#pragma once

#include<Gear/Core/RenderTask.h>

class MyRenderTask :public RenderTask
{
public:

	void recordCommand() override
	{
		context->setDefRenderTarget();

		const float clearValue[4] = {
			cosf(Graphics::getTimeElapsed()) * 0.5f + 0.5f,
			sinf(Graphics::getTimeElapsed()) * 0.5f + 0.5f,
			1.0f,
			1.0f
		};

		context->clearDefRenderTarget(clearValue);
	}

private:

};