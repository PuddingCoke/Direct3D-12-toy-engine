#include<Gear/Core/StaticEffect/LatLongMapToCubeMapEffect.h>

#include<Gear/Core/StaticEffect/Internal/LatLongMapToCubeMapEffectInternal.h>

#include<Gear/Utils/Math.h>

#include<Gear/CompiledShaders/EquirectangularVS.h>

#include<Gear/CompiledShaders/EquirectangularPS.h>

namespace
{
	struct LatLongMapToCubeMapEffectPrivate
	{

		Core::Shader* equirectangularVS;

		Core::Shader* equirectangularPS;

		ComPtr<ID3D12PipelineState> equirectangularR8State;

		ComPtr<ID3D12PipelineState> equirectangularR16State;

		ComPtr<ID3D12PipelineState> equirectangularR32State;

		ImmutableCBuffer* matricesBuffer;

	}pvt;
}

void Core::StaticEffect::LatLongMapToCubeMapEffect::process(GraphicsContext* const context, TextureRenderView* const inputTexture, TextureRenderView* const outputTexture)
{
	switch (outputTexture->getTexture()->getFormat())
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		context->setPipelineState(pvt.equirectangularR8State.Get());
		break;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		context->setPipelineState(pvt.equirectangularR16State.Get());
		break;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		context->setPipelineState(pvt.equirectangularR32State.Get());
		break;
	}

	context->setViewportSimple(outputTexture->getTexture()->getWidth(), outputTexture->getTexture()->getHeight());

	context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->setRenderTargets({ outputTexture->getRTVMipHandle(0) }, {});

	context->setVSConstantBuffer(pvt.matricesBuffer);

	context->setPSConstants({ inputTexture->getAllSRVIndex() }, 0);

	context->transitionResources();

	context->draw(36, 6, 0, 0);
}

void Core::StaticEffect::LatLongMapToCubeMapEffect::Internal::initialize(ResourceManager* const resManager)
{
	pvt.equirectangularVS = new Core::Shader(g_EquirectangularVSBytes, sizeof(g_EquirectangularVSBytes));

	pvt.equirectangularPS = new Core::Shader(g_EquirectangularPSBytes, sizeof(g_EquirectangularPSBytes));

	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = Core::PipelineStateHelper::getDefaultGraphicsDesc();
		desc.InputLayout = {};
		desc.VS = pvt.equirectangularVS->getByteCode();
		desc.PS = pvt.equirectangularPS->getByteCode();
		desc.RasterizerState = Core::PipelineStateHelper::rasterCullNone;
		desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		desc.DepthStencilState.DepthEnable = FALSE;
		desc.DepthStencilState.StencilEnable = FALSE;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		Core::GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pvt.equirectangularR8State));

		desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;

		Core::GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pvt.equirectangularR16State));

		desc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		Core::GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pvt.equirectangularR32State));
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

			const DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(Utils::Math::halfPi, 1.f, 0.1f, 10.f);

			for (uint32_t i = 0; i < 6; i++)
			{
				const DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH({ 0.f,0.f,0.f }, focusPoints[i], upVectors[i]);
				const DirectX::XMMATRIX viewProj = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);

				matrices.matrices[i] = viewProj;
			}
		}

		pvt.matricesBuffer = resManager->createImmutableCBuffer(sizeof(Matrices), &matrices, false);

		pvt.matricesBuffer->getBuffer()->setName(L"LatLongMap To Cubemap Matrices");
	}

	LOGSUCCESS(L"create", LogColor::brightMagenta, L"LatLongMapToCubeMapEffect", LogColor::defaultColor, L"succeeded");
}

void Core::StaticEffect::LatLongMapToCubeMapEffect::Internal::release()
{
	if (pvt.equirectangularVS)
	{
		delete pvt.equirectangularVS;
	}

	if (pvt.equirectangularPS)
	{
		delete pvt.equirectangularPS;
	}

	pvt.equirectangularR8State = nullptr;

	pvt.equirectangularR16State = nullptr;

	pvt.equirectangularR32State = nullptr;

	if (pvt.matricesBuffer)
	{
		delete pvt.matricesBuffer;
	}
}

