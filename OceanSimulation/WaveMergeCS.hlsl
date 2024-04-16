#include"Common.hlsli"

cbuffer TextureIndex : register(b2)
{
    uint DxyzIndex;
    uint normalJacobianIndex;
    uint DyIndex;
    uint DxIndex;
    uint DzIndex;
    uint DyxIndex;
    uint DyzIndex;
    uint DxxIndex;
    uint DzzIndex;
    uint DxzIndex;
}

static RWTexture2D<float4> Dxyz = ResourceDescriptorHeap[DxyzIndex];
static RWTexture2D<float4> normalJacobian = ResourceDescriptorHeap[normalJacobianIndex];

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
 
    float temp = normalJacobian[DTid].w + perframeResource.deltaTime * 0.5 / max(jacobian, 0.5);
    
    jacobian = min(jacobian, temp);
    
    Dxyz[DTid] = float4(lambda * Dx[DTid].x, Dy[DTid].x, lambda * Dz[DTid].x, 1.0);
    
    normalJacobian[DTid] = float4(normalize(float3(-slope.x, 1.0, -slope.y)), jacobian);
}