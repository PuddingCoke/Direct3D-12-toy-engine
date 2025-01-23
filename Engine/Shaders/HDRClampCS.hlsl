#include"Common.hlsli"

cbuffer TextureIndices : register(b2)
{
    uint textureIndex;
}

static RWTexture2D<float4> hdrTexture = ResourceDescriptorHeap[textureIndex];

[numthreads(16, 16, 1)]
void main(const uint2 DTid : SV_DispatchThreadID )
{
    float4 color = hdrTexture[DTid];
    
    if (isnan(color.r) || isinf(color.r))
    {
        color.r = 65504.0;
    }

    if (isnan(color.g) || isinf(color.g))
    {
        color.g = 65504.0;
    }
    
    if (isnan(color.b) || isinf(color.b))
    {
        color.b = 65504.0;
    }
    
    hdrTexture[DTid] = color;
}