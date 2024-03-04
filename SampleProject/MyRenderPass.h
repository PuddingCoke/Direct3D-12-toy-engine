#pragma once

#include<Gear/Core/RenderPass.h>

#include<Gear/Core/Resource/TextureRenderTarget.h>

class MyRenderPass :public RenderPass
{
public:

	MyRenderPass()
	{
		begin();

		myTexture = CreateTextureRenderTarget(TEXTURE_VIEW_CREATE_SRV, "est.png",
			DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, false);

		end();

		RenderEngine::get()->submitRenderPass(this);
	}

	~MyRenderPass()
	{
		delete myTexture;
	}

protected:

	void recordCommand() override
	{
		float clearValue[4] = { sinf(Graphics::getTimeElapsed()) * 0.5f + 0.5f,cosf(Graphics::getTimeElapsed()) * 0.5f + 0.5f,0.f,1.f };
		clearDefRenderTarget(clearValue);
	}

private:

	TextureRenderTarget* myTexture;

};