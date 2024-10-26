#include"Common.hlsli"

cbuffer TextureIndex : register(b2)
{
    uint DxyzJacobianIndex;
    uint DerivativeIndex;
    uint DyIndex;
    uint DxIndex;
    uint DzIndex;
    uint DyxIndex;
    uint DyzIndex;
    uint DxxIndex;
    uint DzzIndex;
    uint DxzIndex;
}

static RWTexture2D<float4> DxyzJacobian = ResourceDescriptorHeap[DxyzJacobianIndex];
static RWTexture2D<float4> Derivative = ResourceDescriptorHeap[DerivativeIndex];

static Texture2D<float2> Dy = ResourceDescriptorHeap[DyIndex];
static Texture2D<float2> Dx = ResourceDescriptorHeap[DxIndex];
static Texture2D<float2> Dz = ResourceDescriptorHeap[DzIndex];
static Texture2D<float2> Dyx = ResourceDescriptorHeap[DyxIndex];
static Texture2D<float2> Dyz = ResourceDescriptorHeap[DyzIndex];
static Texture2D<float2> Dxx = ResourceDescriptorHeap[DxxIndex];
static Texture2D<float2> Dzz = ResourceDescriptorHeap[DzzIndex];
static Texture2D<float2> Dxz = ResourceDescriptorHeap[DxzIndex];

static const float lambda = 1.0;

[numthreads(32, 32, 1)]
void main(uint2 DTid : SV_DispatchThreadID)
{
    float2 slope = float2(Dyx[DTid].x / (1.0 + lambda * Dxx[DTid].x), Dyz[DTid].x / (1.0 + lambda * Dzz[DTid].x));
    
    float jacobian = (1 + lambda * Dxx[DTid].x) * (1 + lambda * Dzz[DTid].x) - lambda * lambda * Dxz[DTid].x * Dxz[DTid].x;
 
    float temp = DxyzJacobian[DTid].w + perframeResource.deltaTime * 0.5 / max(jacobian, 0.5);
    
    jacobian = min(jacobian, temp);
    
    DxyzJacobian[DTid] = float4(lambda * Dx[DTid].x, Dy[DTid].x, lambda * Dz[DTid].x, jacobian);
    
    Derivative[DTid] = float4(Dyx[DTid].x, Dyz[DTid].x, lambda * Dxx[DTid].x, lambda * Dzz[DTid].x);
}