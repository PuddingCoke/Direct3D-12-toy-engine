#include"Common.hlsli"

#define M_PI 3.1415926535897932384626433832795

cbuffer TextureIndex : register(b2)
{
    uint DyIndex;
    uint DxIndex;
    uint DzIndex;
    uint DyxIndex;
    uint DyzIndex;
    uint DxxIndex;
    uint DzzIndex;
    uint DxzIndex;
    uint tildeh0Index;
    uint waveDataIndex;
};

static RWTexture2D<float2> Dy = ResourceDescriptorHeap[DyIndex];
static RWTexture2D<float2> Dx = ResourceDescriptorHeap[DxIndex];
static RWTexture2D<float2> Dz = ResourceDescriptorHeap[DzIndex];
static RWTexture2D<float2> Dyx = ResourceDescriptorHeap[DyxIndex];
static RWTexture2D<float2> Dyz = ResourceDescriptorHeap[DyzIndex];
static RWTexture2D<float2> Dxx = ResourceDescriptorHeap[DxxIndex];
static RWTexture2D<float2> Dzz = ResourceDescriptorHeap[DzzIndex];
static RWTexture2D<float2> Dxz = ResourceDescriptorHeap[DxzIndex];

static Texture2D<float4> tildeh0 = ResourceDescriptorHeap[tildeh0Index];
static Texture2D<float4> waveData = ResourceDescriptorHeap[waveDataIndex];

float2 ComplexMul(float2 a, float2 b)
{
    return float2(a.x * b.x - a.y * b.y, a.y * b.x + a.x * b.y);
}

[numthreads(32, 32, 1)]
void main(uint2 DTid : SV_DispatchThreadID)
{
    float4 wave = waveData[DTid];
    
    float2 htilde0 = tildeh0[DTid].xy;
    
    float2 htilde0mkconj = tildeh0[DTid].zw;
    
    float omegat = wave.w * perframeResource.timeElapsed;
    
    float cos_ = cos(omegat);
    float sin_ = sin(omegat);
    
    float2 c0 = float2(cos_, sin_);
    float2 c1 = float2(cos_, -sin_);
    
    float2 h = ComplexMul(htilde0, c0) + ComplexMul(htilde0mkconj, c1);
    
    float2 ih = float2(-h.y, h.x);
    
    Dy[DTid] = h;
    Dyx[DTid] = ih * wave.x;
    Dyz[DTid] = ih * wave.z;
    Dx[DTid] = ih * wave.x * wave.y;
    Dz[DTid] = ih * wave.z * wave.y;
    Dxx[DTid] = -h * wave.x * wave.x * wave.y;
    Dzz[DTid] = -h * wave.z * wave.z * wave.y;
    Dxz[DTid] = -h * wave.x * wave.z * wave.y;
}