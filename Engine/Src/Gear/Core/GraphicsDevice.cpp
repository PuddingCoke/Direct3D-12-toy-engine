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
		std::cout<<"[class GraphicsDevice] create d3d12 device at feature level 12_2 succeeded\n";
	}
}

void GraphicsDevice::checkFeatureSupport()
{
	std::cout << "[class GraphicsDevice] following are feature support data\n";
	
	//check resouce binding tier and typed uav load
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS option = {};
		
		device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &option, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS));

		{
			std::cout << "[class GraphicsDevice] resource binding tier ";

			switch (option.ResourceBindingTier)
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
			std::cout << "[class GraphicsDevice] typed uav load ";

			if (option.TypedUAVLoadAdditionalFormats)
			{
				std::cout << "yes";
			}
			else
			{
				std::cout << "no";
			}

			std::cout << "\n";
		}
	}

	//check raytracing support
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 option = {};

		device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &option, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS5));
	
		std::cout << "[class GraphicsDevice] raytracing support ";

		switch (option.RaytracingTier)
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
