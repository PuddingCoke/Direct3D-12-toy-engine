#include"Common.hlsli"

#define M_PI 3.1415926535897932384626433832795

cbuffer TextureIndices : register(b2)
{
    uint DyIndex;
    uint DxIndex;
    uint DzIndex;
    uint DyxIndex;
    uint DyzIndex;
    uint DxxIndex;
    uint DzzIndex;
    uint DxzIndex;
    uint waveDataTextureIndex;
    uint waveSpectrumTextureIndex;
}

static RWTexture2D<float2> Dy = ResourceDescriptorHeap[DyIndex];

static RWTexture2D<float2> Dx = ResourceDescriptorHeap[DxIndex];

static RWTexture2D<float2> Dz = ResourceDescriptorHeap[DzIndex];

static RWTexture2D<float2> Dyx = ResourceDescriptorHeap[DyxIndex];

static RWTexture2D<float2> Dyz = ResourceDescriptorHeap[DyzIndex];

static RWTexture2D<float2> Dxx = ResourceDescriptorHeap[DxxIndex];

static RWTexture2D<float2> Dzz = ResourceDescriptorHeap[DzzIndex];

static RWTexture2D<float2> Dxz = ResourceDescriptorHeap[DxzIndex];

static Texture2D<float4> waveDataTexture = ResourceDescriptorHeap[waveDataTextureIndex];

static Texture2D<float4> waveSpectrumTexture = ResourceDescriptorHeap[waveSpectrumTextureIndex];

float2 ComplexMul(const float2 a, const float2 b)
{
    return float2(a.x * b.x - a.y * b.y, a.y * b.x + a.x * b.y);
}

[numthreads(8, 8, 1)]
void main(const uint2 DTid : SV_DispatchThreadID)
{
    const float4 waveData = waveDataTexture[DTid];
    
    const float4 waveSpectrum = waveSpectrumTexture[DTid];
    
    const float2 tildeh0k = waveSpectrum.xy;
    
    const float2 tildeh0MinuskConj = waveSpectrum.zw;
    
    const float omegat = waveData.w * perframeResource.timeElapsed;
    
    const float cos_ = cos(omegat);
    
    const float sin_ = sin(omegat);
    
    const float2 c0 = float2(cos_, sin_);
    
    const float2 c1 = float2(cos_, -sin_);
    
    const float2 h = ComplexMul(tildeh0k, c0) + ComplexMul(tildeh0MinuskConj, c1);
    
    const float2 ih = float2(-h.y, h.x);
    
    Dy[DTid] = h;
    
    Dyx[DTid] = ih * waveData.x;
    
    Dyz[DTid] = ih * waveData.z;
    
    Dx[DTid] = ih * waveData.x * waveData.y;
    
    Dz[DTid] = ih * waveData.z * waveData.y;
    
    Dxx[DTid] = -h * waveData.x * waveData.x * waveData.y;
    
    Dzz[DTid] = -h * waveData.z * waveData.z * waveData.y;
    
    Dxz[DTid] = -h * waveData.x * waveData.z * waveData.y;
}