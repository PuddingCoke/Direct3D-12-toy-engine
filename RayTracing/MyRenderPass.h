#pragma once

#include<Gear/Core/RenderPass.h>

#include<Gear/Core/Shader.h>

#include<Gear/Utils/Math.h>
#include<Gear/Utils/Random.h>
#include<Gear/Utils/Color.h>

#include<Gear/Core/Resource/CounterBufferView.h>

class MyRenderPass :public RenderPass
{
public:

	MyRenderPass()
	{

	}

	~MyRenderPass()
	{
		delete texture;
	}

protected:

	void recordCommand() override
	{
		context->setDefRenderTarget();

		const float color[4] = { 0.f,1.f,1.f,1.f };

		context->clearDefRenderTarget(color);
	}

private:

};