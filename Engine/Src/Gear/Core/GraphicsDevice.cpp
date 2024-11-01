#include<Gear/Core/GraphicsDevice.h>

GraphicsDevice* GraphicsDevice::instance = nullptr;

ID3D12Device9* GraphicsDevice::get()
{
	return instance->device.Get();
}

GraphicsDevice::GraphicsDevice(IUnknown* const adapter)
{
	if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&device))))
	{
		std::cout << "[class GraphicsDevice] create d3d12 device at feature level 12_2 succeeded\n";
	}
}

void GraphicsDevice::checkFeatureSupport()
{
	std::cout << "[class GraphicsDevice] following are feature support data\n";

	CD3DX12FeatureSupport features;

	features.Init(device.Get());

	{
		const D3D12_RESOURCE_BINDING_TIER resourceBindingTier = features.ResourceBindingTier();

		std::cout << "[class GraphicsDevice] resource binding tier ";

		switch (resourceBindingTier)
		{
		default:
		case D3D12_RESOURCE_BINDING_TIER_1:
			std::cout << "D3D12_RESOURCE_BINDING_TIER_1";
			break;
		case D3D12_RESOURCE_BINDING_TIER_2:
			std::cout << "D3D12_RESOURCE_BINDING_TIER_2";
			break;
		case D3D12_RESOURCE_BINDING_TIER_3:
			std::cout << "D3D12_RESOURCE_BINDING_TIER_3";
			break;
		}

		std::cout << "\n";
	}

	{
		const D3D_SHADER_MODEL shaderModel = features.HighestShaderModel();

		std::cout << "[class GraphicsDevice] highest shader model ";

		switch (shaderModel)
		{
		default:
		case D3D_SHADER_MODEL_5_1:
			std::cout << "D3D_SHADER_MODEL_5_1";
			break;
		case D3D_SHADER_MODEL_6_0:
			std::cout << "D3D_SHADER_MODEL_6_0";
			break;
		case D3D_SHADER_MODEL_6_1:
			std::cout << "D3D_SHADER_MODEL_6_1";
			break;
		case D3D_SHADER_MODEL_6_2:
			std::cout << "D3D_SHADER_MODEL_6_2";
			break;
		case D3D_SHADER_MODEL_6_3:
			std::cout << "D3D_SHADER_MODEL_6_3";
			break;
		case D3D_SHADER_MODEL_6_4:
			std::cout << "D3D_SHADER_MODEL_6_4";
			break;
		case D3D_SHADER_MODEL_6_5:
			std::cout << "D3D_SHADER_MODEL_6_5";
			break;
		case D3D_SHADER_MODEL_6_6:
			std::cout << "D3D_SHADER_MODEL_6_6";
			break;
		case D3D_SHADER_MODEL_6_7:
			std::cout << "D3D_SHADER_MODEL_6_7";
			break;
		case D3D_SHADER_MODEL_6_8:
			std::cout << "D3D_SHADER_MODEL_6_8";
			break;
		}

		std::cout << "\n";
	}

	{
		const BOOL typedUAVLoad = features.TypedUAVLoadAdditionalFormats();

		std::cout << "[class GraphicsDevice] typed uav load " << (typedUAVLoad ? "yes" : "no") << "\n";
	}

	{
		const D3D12_RAYTRACING_TIER rayTracingTier = features.RaytracingTier();

		std::cout << "[class GraphicsDevice] raytracing tier ";

		switch (rayTracingTier)
		{
		default:
		case D3D12_RAYTRACING_TIER_NOT_SUPPORTED:
			std::cout << "D3D12_RAYTRACING_TIER_NOT_SUPPORTED";
			break;
		case D3D12_RAYTRACING_TIER_1_0:
			std::cout << "D3D12_RAYTRACING_TIER_1_0";
			break;
		case D3D12_RAYTRACING_TIER_1_1:
			std::cout << "D3D12_RAYTRACING_TIER_1_1";
			break;
		}

		std::cout << "\n";
	}




}
