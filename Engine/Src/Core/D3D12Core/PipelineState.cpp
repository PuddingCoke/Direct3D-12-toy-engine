#include<Gear/Core/D3D12Core/PipelineState.h>

Gear::Core::D3D12Core::PipelineState::PipelineState(const ComPtr<ID3D12PipelineState>& pipelineState,const RootSignature* const rootSignature, const PipelineStateType pipelineStateType) :
	pipelineState(pipelineState), rootSignature(rootSignature), pipelineStateType(pipelineStateType)
{
}

Gear::Core::D3D12Core::PipelineState::~PipelineState()
{
}

const Gear::Core::D3D12Core::RootSignature* Gear::Core::D3D12Core::PipelineState::getRootSignature() const
{
	return rootSignature;
}

ID3D12PipelineState* Gear::Core::D3D12Core::PipelineState::get() const
{
	return pipelineState.Get();
}

Gear::Core::D3D12Core::PipelineState::PipelineStateType Gear::Core::D3D12Core::PipelineState::getPipelineStateType() const
{
	return pipelineStateType;
}
