#include"Common.hlsli"

cbuffer TextureIndices : register(b2)
{
    uint displacementTextureIndex;
    uint derivativeTextureIndex;
    uint jacobianTextureIndex;
    uint DyIndex;
    uint DxIndex;
    uint DzIndex;
    uint DyxIndex;
    uint DyzIndex;
    uint DxxIndex;
    uint DzzIndex;
    uint DxzIndex;
}

static RWTexture2D<float4> displacementTexture = ResourceDescriptorHeap[displacementTextureIndex];

static RWTexture2D<float4> derivativeTexture = ResourceDescriptorHeap[derivativeTextureIndex];

static RWTexture2D<float> jacobianTexture = ResourceDescriptorHeap[jacobianTextureIndex];

static Texture2D<float2> Dy = ResourceDescriptorHeap[DyIndex];

static Texture2D<float2> Dx = ResourceDescriptorHeap[DxIndex];

static Texture2D<float2> Dz = ResourceDescriptorHeap[DzIndex];

static Texture2D<float2> Dyx = ResourceDescriptorHeap[DyxIndex];

static Texture2D<float2> Dyz = ResourceDescriptorHeap[DyzIndex];

static Texture2D<float2> Dxx = ResourceDescriptorHeap[DxxIndex];

static Texture2D<float2> Dzz = ResourceDescriptorHeap[DzzIndex];

static Texture2D<float2> Dxz = ResourceDescriptorHeap[DxzIndex];

static const float lambda = 1.0;

[numthreads(8, 8, 1)]
void main(const uint2 DTid : SV_DispatchThreadID )
{
    displacementTexture[DTid] = float4(lambda * Dx[DTid].x, Dy[DTid].x, lambda * Dz[DTid].x, 1.0);
    
    derivativeTexture[DTid] = float4(Dyx[DTid].x, Dyz[DTid].x, lambda * Dxx[DTid].x, lambda * Dzz[DTid].x);
    
    float jacobian = (1.0 + lambda * Dxx[DTid].x) * (1.0 + lambda * Dzz[DTid].x) - lambda * lambda * Dxz[DTid].x * Dxz[DTid].x;
    
    const float temp = jacobianTexture[DTid] + perframeResource.deltaTime * 0.5 / max(jacobian, 0.5);
    
    jacobian = min(jacobian, temp);
    
    jacobianTexture[DTid] = jacobian;
}