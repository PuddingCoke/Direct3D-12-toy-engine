#include"Common.hlsli"

cbuffer TextureIndices : register(b2)
{
    uint textureIdx;
}

static Texture2D texture = ResourceDescriptorHeap[textureIdx];

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    return texture.Sample(linearClampSampler, texCoord);
}