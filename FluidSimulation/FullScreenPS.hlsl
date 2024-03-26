#include"Common.hlsli"

cbuffer TextureIndices : register(b1)
{
    uint textureIdx;
}

static const Texture2D<float4> tex = ResourceDescriptorHeap[textureIdx];

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    return tex.Sample(linearClampSampler, texCoord);
}