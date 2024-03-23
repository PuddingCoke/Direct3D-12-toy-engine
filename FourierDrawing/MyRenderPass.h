#pragma once

#include<Gear/Core/RenderPass.h>

#include<Gear/Core/Shader.h>

#include<Gear/Core/Resource/TextureRenderTarget.h>

#include<Gear/Utils/Math.h>
#include<Gear/Utils/Random.h>

#include<Gear/Core/Gear2D/PrimitiveBatch.h>

#include<DirectXColors.h>

#include"Epicycle.h"

#include"json.hpp"

using nlohmann::json;

class MyRenderPass :public RenderPass
{
public:

	const double tPi = 6.28318530717958647692;

	const double startX = 0;

	const double startY = 0;

	double t = 0;

	double x;

	double y;

	double preX;

	double preY;

	Epicycle* epicycles;

	size_t length;

	double lastX, lastY;

	size_t curFrame;

	static constexpr int interval = 5;

	PrimitiveBatch* pBatch;

	TextureRenderTarget* renderTexture;

	ComPtr<ID3D12PipelineState> fullScreenPipelineState;

	Shader* fullScreenPS;

	const struct Color
	{
		float r;
		float g;
		float b;
	}color{ 0.f,0.f,0.f };

	bool connected;

	MyRenderPass() :
		pBatch(PrimitiveBatch::create(DXGI_FORMAT_R8G8B8A8_UNORM, context)),
		renderTexture(CreateTextureRenderTarget(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, false, true,
			DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R8G8B8A8_UNORM)),
		curFrame(0),
		connected(false)
	{
		fullScreenPS = new Shader(Utils::getRootFolder() + "FullScreenPS.cso");

		begin();

		context->setRenderTargets({ renderTexture->getRTVMipHandle(0) }, {});

		context->clearRenderTarget(renderTexture->getRTVMipHandle(0), DirectX::Colors::Transparent);

		end();

		RenderEngine::get()->submitRenderPass(this);

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
			desc.InputLayout = {};
			desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
			desc.VS = Shader::fullScreenVS->getByteCode();
			desc.PS = fullScreenPS->getByteCode();
			desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			desc.BlendState = States::defBlendDesc;
			desc.DepthStencilState.DepthEnable = FALSE;
			desc.DepthStencilState.StencilEnable = FALSE;
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = Graphics::BackBufferFormat;
			desc.SampleDesc.Count = 1;

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&fullScreenPipelineState));
		}

		pBatch->setLineWidth(1.5f);

		const std::string filePath = "dft_data.json";
		std::ifstream ifs(filePath);
		json dftData = json::parse(ifs);
		ifs.close();

		length = dftData.size();

		double real;
		double imag;
		double radius;

		epicycles = new Epicycle[length];

		const double scale = 1.0;

		for (size_t i = 0; i < length; i++)
		{
			real = dftData[i]["x"].get<double>() * scale;
			imag = dftData[i]["y"].get<double>() * scale;
			radius = sqrt(real * real + imag * imag);
			epicycles[i] = Epicycle(radius, (double)i * tPi / length, atan2(imag, real));
		}

		x = startX;
		y = startY;

		for (size_t i = 0; i < length; i++)
		{
			epicycles[i].set(t);
			preX = x;
			preY = y;
			x += epicycles[i].re;
			y += epicycles[i].im;
		}

		lastX = x;
		lastY = y;

		t += 1.0;

		curFrame++;

		x = startX;
		y = startY;

		for (size_t i = 0; i < length; i++)
		{
			epicycles[i].set(length - 1);
			preX = x;
			preY = y;
			x += epicycles[i].re;
			y += epicycles[i].im;
		}

		if (sqrt((x - lastX) * (x - lastX) + (y - lastY) * (y - lastY)) < 50.0)
		{
			connected = true;
		}

		Camera::setProj(DirectX::XMMatrixOrthographicOffCenterLH(0.f, (float)Graphics::getWidth(), 0, (float)Graphics::getHeight(), -1.f, 1.f));
	}

	~MyRenderPass()
	{
		delete[] epicycles;
		delete renderTexture;
		delete pBatch;
		delete fullScreenPS;
	}

protected:

	void recordCommand() override
	{
		context->setDefRenderTarget();
		context->clearDefRenderTarget(DirectX::Colors::White);
		context->setViewport(Graphics::getWidth(), Graphics::getHeight());
		context->setScissorRect(0, 0, Graphics::getWidth(), Graphics::getHeight());

		x = startX;
		y = startY;

		pBatch->begin();
		for (size_t i = 0; i < length; i++)
		{
			epicycles[i].set(t);
			preX = x;
			preY = y;
			x += epicycles[i].re;
			y += epicycles[i].im;
			pBatch->drawCircle(preX, preY, epicycles[i].length, 0, 0, 0, 0.25f);
			pBatch->drawLine(preX, preY, x, y, 0, 0, 0, 0.25f);
		}
		pBatch->end();

		context->setRenderTargets({ renderTexture->getRTVMipHandle(0) }, {});

		pBatch->begin();

		if (curFrame++ == length)
		{
			if (connected)
			{
				pBatch->drawRoundCapLine(lastX, lastY, x, y, 4.f, color.r, color.g, color.b);
			}
		}
		else
		{
			pBatch->drawRoundCapLine(lastX, lastY, x, y, 4.f, color.r, color.g, color.b);
		}

		lastX = x;
		lastY = y;

		t += 1.0;

		for (int intervalCount = 0; intervalCount < interval; intervalCount++)
		{
			x = startX;
			y = startY;

			for (size_t i = 0; i < length; i++)
			{
				epicycles[i].set(t);
				preX = x;
				preY = y;
				x += epicycles[i].re;
				y += epicycles[i].im;
			}

			if (curFrame++ == length)
			{
				if (connected)
				{
					pBatch->drawRoundCapLine(lastX, lastY, x, y, 4.f, color.r, color.g, color.b);
				}
			}
			else
			{
				pBatch->drawRoundCapLine(lastX, lastY, x, y, 4.f, color.r, color.g, color.b);
			}

			lastX = x;
			lastY = y;

			t += 1.0;
		}

		pBatch->end();

		context->setDefRenderTarget();

		context->setGraphicsConstants({ renderTexture->getAllSRVIndex() }, 0);

		context->setPipelineState(fullScreenPipelineState.Get());

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->draw(3, 1, 0, 0);
	}

};