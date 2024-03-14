#include"Common.hlsli"

cbuffer TextureIndex : register(b1)
{
    uint textureIdx;
};

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    Texture2D<float4> tex = ResourceDescriptorHeap[textureIdx];
    
    return tex.Sample(linearClampSampler, texCoord);
}