#pragma once

#include<Gear/Core/RenderPass.h>
#include<Gear/Core/Shader.h>

#include<Gear/Core/Resource/TextureRenderView.h>

class MyRenderPass :public RenderPass
{
public:

	MyRenderPass() :
		computeCS(new Shader(Utils::getRootFolder() + "ComputeCS.cso")),
		originTexture(ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_UNORM, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_UNKNOWN))
	{
		PipelineState::createComputeState(&computeState, computeCS);
	}

	~MyRenderPass()
	{
		delete computeCS;

		delete originTexture;
	}

	void imGUICall() override
	{
		ImGui::Begin("Simulation Parameters");
		ImGui::SliderFloat("scale", &param.scale, 0.f, 1.f);
		ImGui::End();
	}

protected:

	void recordCommand() override
	{
		if (Mouse::onScroll())
		{
			if (Mouse::getWheelDelta() > 0.f)
			{
				param.scale *= 0.85;
			}
			else
			{
				param.scale /= 0.85;
			}
		}

		if (Mouse::getLeftDown() && Mouse::onMove())
		{
			param.location.x -= Graphics::getDeltaTime() * Mouse::getDX() * param.scale;

			param.location.y += Graphics::getDeltaTime() * Mouse::getDY() * param.scale;
		}

		context->setPipelineState(computeState.Get());

		context->setCSConstants({ originTexture->getUAVMipIndex(0) }, 0);

		context->setCSConstants(sizeof(SimulationParam) / 4, &param, 1);

		context->transitionResources();

		context->dispatch(Graphics::getWidth() / 16, Graphics::getHeight() / 9, 1);

		context->uavBarrier({ originTexture->getTexture() });

		blit(originTexture);
	}

private:

	Shader* computeCS;

	ComPtr<ID3D12PipelineState> computeState;

	TextureRenderView* originTexture;

	struct SimulationParam
	{
		DirectX::XMFLOAT2 location = { 0.f,0.f };
		float scale = { 1.f };
		const DirectX::XMFLOAT2 texelSize = { 1.f / static_cast<float>(Graphics::getWidth()),1.f / static_cast<float>(Graphics::getHeight()) };
	} param;
};