#include<Gear/Core/D3D12Core/RootSignature.h>

Gear::Core::D3D12Core::RootSignature::RootSignature(const uint32_t numVSConstants, const uint32_t numHSConstants, const uint32_t numDSConstants, const uint32_t numGSConstants, const uint32_t numPSConstants, const uint32_t numCSConstants, CD3DX12_STATIC_SAMPLER_DESC* const samplerDescs, const uint32_t samplerCount, const D3D12_ROOT_SIGNATURE_FLAGS signatureFlags) :
	numConstants{ numVSConstants,numHSConstants,numDSConstants,numGSConstants,numPSConstants,numCSConstants }, rootParameterStartIndices{}
{
	const uint32_t numRootParameters = 2u
		+ (static_cast<bool>(numVSConstants)
			+ static_cast<bool>(numHSConstants)
			+ static_cast<bool>(numDSConstants)
			+ static_cast<bool>(numGSConstants)
			+ static_cast<bool>(numPSConstants)
			+ static_cast<bool>(numCSConstants)) * 2u;

	std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;

	rootParameters.resize(numRootParameters);

	//由引擎决定的所有着色器可见的全局常量缓冲
	rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE, D3D12_SHADER_VISIBILITY_ALL);
	//由用户决定的所有着色器可见的全局常量缓冲
	rootParameters[1].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE, D3D12_SHADER_VISIBILITY_ALL);

	{
		uint32_t rootParameterIndex = 2u;

		auto setShaderRootParameter = [this](CD3DX12_ROOT_PARAMETER1* const rootParameters, uint32_t& rootParameterIndex,
			const ShaderType shaderType, const uint32_t numConstants)
			{
				rootParameterStartIndices[static_cast<uint32_t>(shaderType)] = rootParameterIndex;

				D3D12_SHADER_VISIBILITY shaderVisibility;

				switch (shaderType)
				{
				case ShaderType::VERTEX:
					shaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
					break;
				case ShaderType::HULL:
					shaderVisibility = D3D12_SHADER_VISIBILITY_HULL;
					break;
				case ShaderType::DOMAIN:
					shaderVisibility = D3D12_SHADER_VISIBILITY_DOMAIN;
					break;
				case ShaderType::GEOMETRY:
					shaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
					break;
				case ShaderType::PIXEL:
					shaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
					break;
				case ShaderType::COMPUTE:
					shaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
					break;
				default:
					break;
				}

				rootParameters[rootParameterIndex++].InitAsConstants(numConstants, 2, 0, shaderVisibility);

				rootParameters[rootParameterIndex++].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE, shaderVisibility);
			};

		if (numVSConstants)
			setShaderRootParameter(rootParameters.data(), rootParameterIndex, ShaderType::VERTEX, numVSConstants);

		if (numHSConstants)
			setShaderRootParameter(rootParameters.data(), rootParameterIndex, ShaderType::HULL, numHSConstants);

		if (numDSConstants)
			setShaderRootParameter(rootParameters.data(), rootParameterIndex, ShaderType::DOMAIN, numDSConstants);

		if (numGSConstants)
			setShaderRootParameter(rootParameters.data(), rootParameterIndex, ShaderType::GEOMETRY, numGSConstants);

		if (numPSConstants)
			setShaderRootParameter(rootParameters.data(), rootParameterIndex, ShaderType::PIXEL, numPSConstants);

		if (numCSConstants)
			setShaderRootParameter(rootParameters.data(), rootParameterIndex, ShaderType::COMPUTE, numCSConstants);
	}

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};

	rootSignatureDesc.Init_1_1(numRootParameters, rootParameters.data(), samplerCount, samplerDescs, signatureFlags);

	ComPtr<ID3DBlob> signature;

	ComPtr<ID3DBlob> error;

	D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_1, &signature, &error);

	CHECKERROR(GraphicsDevice::get()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
}

Gear::Core::D3D12Core::RootSignature::~RootSignature()
{
}

ID3D12RootSignature* Gear::Core::D3D12Core::RootSignature::get() const
{
	return rootSignature.Get();
}

uint32_t Gear::Core::D3D12Core::RootSignature::getReservedGlobalConstantBufferParameterIndex()
{
	return 0u;
}

uint32_t Gear::Core::D3D12Core::RootSignature::getUserDefinedGlobalConstantBufferParameterIndex()
{
	return 1u;
}

uint32_t Gear::Core::D3D12Core::RootSignature::getVSConstantsParameterIndex() const
{
	return getShaderConstantsParameterIndex(ShaderType::VERTEX);
}

uint32_t Gear::Core::D3D12Core::RootSignature::getVSConstantBufferParameterIndex() const
{
	return getVSConstantsParameterIndex() + 1u;
}

uint32_t Gear::Core::D3D12Core::RootSignature::getHSConstantsParameterIndex() const
{
	return getShaderConstantsParameterIndex(ShaderType::HULL);
}

uint32_t Gear::Core::D3D12Core::RootSignature::getHSConstantBufferParameterIndex() const
{
	return getHSConstantsParameterIndex() + 1u;
}

uint32_t Gear::Core::D3D12Core::RootSignature::getDSConstantsParameterIndex() const
{
	return getShaderConstantsParameterIndex(ShaderType::DOMAIN);
}

uint32_t Gear::Core::D3D12Core::RootSignature::getDSConstantBufferParameterIndex() const
{
	return getDSConstantsParameterIndex() + 1u;
}

uint32_t Gear::Core::D3D12Core::RootSignature::getGSConstantsParameterIndex() const
{
	return getShaderConstantsParameterIndex(ShaderType::GEOMETRY);
}

uint32_t Gear::Core::D3D12Core::RootSignature::getGSConstantBufferParameterIndex() const
{
	return getGSConstantsParameterIndex() + 1u;
}

uint32_t Gear::Core::D3D12Core::RootSignature::getPSConstantsParameterIndex() const
{
	return getShaderConstantsParameterIndex(ShaderType::PIXEL);
}

uint32_t Gear::Core::D3D12Core::RootSignature::getPSConstantBufferParameterIndex() const
{
	return getPSConstantsParameterIndex() + 1u;
}

uint32_t Gear::Core::D3D12Core::RootSignature::getCSConstantsParameterIndex() const
{
	return getShaderConstantsParameterIndex(ShaderType::COMPUTE);
}

uint32_t Gear::Core::D3D12Core::RootSignature::getCSConstantBufferParameterIndex() const
{
	return getCSConstantsParameterIndex() + 1u;
}

uint32_t Gear::Core::D3D12Core::RootSignature::getShaderConstantsParameterIndex(const ShaderType shaderType) const
{
	const uint32_t index = static_cast<uint32_t>(shaderType);

#ifdef _DEBUG
	if (rootParameterStartIndices[index] == 0)
	{
		std::wstring errorString = L"there is no root parameter for ";

		switch (shaderType)
		{
		case ShaderType::VERTEX:
			errorString += L"vertex";
			break;
		case ShaderType::HULL:
			errorString += L"hull";
			break;
		case ShaderType::DOMAIN:
			errorString += L"domain";
			break;
		case ShaderType::GEOMETRY:
			errorString += L"geometry";
			break;
		case ShaderType::PIXEL:
			errorString += L"pixel";
			break;
		case ShaderType::COMPUTE:
			errorString += L"compute";
			break;
		default:
			break;
		}

		errorString += L" shader";

		LOGERROR(errorString);
	}
#endif // _DEBUG

	return rootParameterStartIndices[index];
}
