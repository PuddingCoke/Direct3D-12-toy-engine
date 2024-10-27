#include"Common.hlsli"

struct PixelInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

cbuffer TextureIndices : register(b2)
{
    uint displacementTextureIndex;
    uint derivativeTextureIndex;
    uint jacobianTextureIndex;
}

static Texture2D<float4> displacementTexture = ResourceDescriptorHeap[displacementTextureIndex];

static Texture2D<float4> derivativeTexture = ResourceDescriptorHeap[derivativeTextureIndex];

static Texture2D<float> jacobianTexture = ResourceDescriptorHeap[jacobianTextureIndex];

float4 main(PixelInput input) : SV_TARGET
{
    return float4(input.uv, 1.0f, 1.0f);
}