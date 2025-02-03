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
		Logger::get()->logSuccess("[", typeid(*this).name(), "]", "create d3d12 device with feature level", Logger::brightMagenta, "D3D_FEATURE_LEVEL_12_2", Logger::resetColor(), "succeeded");
	}
	else
	{
		LOGERROR("your graphics card should support D3D_FEATURE_LEVEL_12_2");
	}
}

void GraphicsDevice::checkFeatureSupport() const
{
	LOGENGINE("following are feature support data");

	CD3DX12FeatureSupport features;

	features.Init(device.Get());

	{
		const D3D12_RESOURCE_BINDING_TIER resourceBindingTier = features.ResourceBindingTier();

		std::wstring bindingTierString;

		switch (resourceBindingTier)
		{
		default:
		case D3D12_RESOURCE_BINDING_TIER_1:
			bindingTierString = L"D3D12_RESOURCE_BINDING_TIER_1";
			break;
		case D3D12_RESOURCE_BINDING_TIER_2:
			bindingTierString = L"D3D12_RESOURCE_BINDING_TIER_2";
			break;
		case D3D12_RESOURCE_BINDING_TIER_3:
			bindingTierString = L"D3D12_RESOURCE_BINDING_TIER_3";
			break;
		}

		LOGENGINE("resource binding tier", Logger::brightMagenta, bindingTierString);
	}

	{
		const D3D_SHADER_MODEL shaderModel = features.HighestShaderModel();

		std::wstring shaderModelString;

		switch (shaderModel)
		{
		default:
		case D3D_SHADER_MODEL_5_1:
			shaderModelString = L"D3D_SHADER_MODEL_5_1";
			break;
		case D3D_SHADER_MODEL_6_0:
			shaderModelString = L"D3D_SHADER_MODEL_6_0";
			break;
		case D3D_SHADER_MODEL_6_1:
			shaderModelString = L"D3D_SHADER_MODEL_6_1";
			break;
		case D3D_SHADER_MODEL_6_2:
			shaderModelString = L"D3D_SHADER_MODEL_6_2";
			break;
		case D3D_SHADER_MODEL_6_3:
			shaderModelString = L"D3D_SHADER_MODEL_6_3";
			break;
		case D3D_SHADER_MODEL_6_4:
			shaderModelString = L"D3D_SHADER_MODEL_6_4";
			break;
		case D3D_SHADER_MODEL_6_5:
			shaderModelString = L"D3D_SHADER_MODEL_6_5";
			break;
		case D3D_SHADER_MODEL_6_6:
			shaderModelString = L"D3D_SHADER_MODEL_6_6";
			break;
		case D3D_SHADER_MODEL_6_7:
			shaderModelString = L"D3D_SHADER_MODEL_6_7";
			break;
		case D3D_SHADER_MODEL_6_8:
			shaderModelString = L"D3D_SHADER_MODEL_6_8";
			break;
		}

		LOGENGINE("highest supported shader model", Logger::brightMagenta, shaderModelString);

		if (shaderModel < D3D_SHADER_MODEL_6_6)
		{
			LOGERROR("your graphics card should at least support D3D_SHADER_MODEL_6_6");
		}
	}

	{
		const BOOL typedUAVLoad = features.TypedUAVLoadAdditionalFormats();

		LOGENGINE("typed uav load", (typedUAVLoad ? Logger::brightGreen : Logger::brightRed), (typedUAVLoad ? "yes" : "no"));

		if (!typedUAVLoad)
		{
			LOGERROR("your graphics card should support typed uav load");
		}
	}

	{
		const D3D12_RAYTRACING_TIER rayTracingTier = features.RaytracingTier();

		std::wstring raytracingTierString;

		switch (rayTracingTier)
		{
		default:
		case D3D12_RAYTRACING_TIER_NOT_SUPPORTED:
			raytracingTierString = L"D3D12_RAYTRACING_TIER_NOT_SUPPORTED";
			break;
		case D3D12_RAYTRACING_TIER_1_0:
			raytracingTierString = L"D3D12_RAYTRACING_TIER_1_0";
			break;
		case D3D12_RAYTRACING_TIER_1_1:
			raytracingTierString = L"D3D12_RAYTRACING_TIER_1_1";
			break;
		}

		LOGENGINE("raytracing tier", Logger::brightMagenta, raytracingTierString);
	}

}
