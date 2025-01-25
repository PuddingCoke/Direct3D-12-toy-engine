#include<Gear/Core/StaticEffect/LatLongMapToCubeMapEffect.h>

#include<Gear/CompiledShaders/EquirectangularVS.h>

#include<Gear/CompiledShaders/EquirectangularPS.h>

LatLongMapToCubeMapEffect* LatLongMapToCubeMapEffect::instance = nullptr;

LatLongMapToCubeMapEffect* LatLongMapToCubeMapEffect::get()
{
	return instance;
}

void LatLongMapToCubeMapEffect::process(GraphicsContext* const context, TextureRenderView* const inputTexture, TextureRenderView* const outputTexture)
{
	switch (outputTexture->getTexture()->getFormat())
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		context->setPipelineState(equirectangularR8State.Get());
		break;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		context->setPipelineState(equirectangularR16State.Get());
		break;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		context->setPipelineState(equirectangularR32State.Get());
		break;
	}

	context->setViewportSimple(outputTexture->getTexture()->getWidth(), outputTexture->getTexture()->getHeight());

	context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->setRenderTargets({ outputTexture->getRTVMipHandle(0) }, {});

	context->setVSConstantBuffer(matricesBuffer);

	context->setPSConstants({ inputTexture->getAllSRVIndex() }, 0);

	context->transitionResources();

	context->draw(36, 6, 0, 0);
}

LatLongMapToCubeMapEffect::LatLongMapToCubeMapEffect(ResourceManager* const resManager) :
	equirectangularVS(new Shader(g_EquirectangularVSBytes, sizeof(g_EquirectangularVSBytes))),
	equirectangularPS(new Shader(g_EquirectangularPSBytes, sizeof(g_EquirectangularPSBytes)))
{
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = {};
		desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
		desc.VS = equirectangularVS->getByteCode();
		desc.PS = equirectangularPS->getByteCode();
		desc.RasterizerState = States::rasterCullNone;
		desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		desc.DepthStencilState.DepthEnable = FALSE;
		desc.DepthStencilState.StencilEnable = FALSE;
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&equirectangularR8State));

		desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&equirectangularR16State));

		desc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&equirectangularR32State));
	}

	{
		struct Matrices
		{
			DirectX::XMMATRIX matrices[6];
			DirectX::XMFLOAT4 padding[8];
		} matrices{};

		{
			const DirectX::XMVECTOR focusPoints[6] =
			{
				{1.0f,  0.0f,  0.0f},
				{-1.0f,  0.0f,  0.0f},
				{0.0f,  1.0f,  0.0f},
				{0.0f, -1.0f,  0.0f},
				{0.0f,  0.0f,  1.0f},
				{0.0f,  0.0f, -1.0f}
			};
			const DirectX::XMVECTOR upVectors[6] =
			{
				{0.0f, 1.0f,  0.0f},
				{0.0f, 1.0f,  0.0f},
				{0.0f,  0.0f,  -1.0f},
				{0.0f,  0.0f, 1.0f},
				{0.0f, 1.0f,  0.0f},
				{0.0f, 1.0f,  0.0f}
			};

			const DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(Math::pi / 2.f, 1.f, 0.1f, 10.f);

			for (uint32_t i = 0; i < 6; i++)
			{
				const DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH({ 0.f,0.f,0.f }, focusPoints[i], upVectors[i]);
				const DirectX::XMMATRIX viewProj = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);

				matrices.matrices[i] = viewProj;
			}
		}

		matricesBuffer = resManager->createConstantBuffer(sizeof(Matrices), false, &matrices, false);
	}

	LOGSUCCESS("create", Logger::brightMagenta, "LatLongMapToCubeMapEffect", Logger::resetColor(), "succeeded");
}

LatLongMapToCubeMapEffect::~LatLongMapToCubeMapEffect()
{
	if (equirectangularVS)
	{
		delete equirectangularVS;
	}

	if (equirectangularPS)
	{
		delete equirectangularPS;
	}

	if (matricesBuffer)
	{
		delete matricesBuffer;
	}
}
