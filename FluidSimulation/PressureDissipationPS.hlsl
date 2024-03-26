#include"Common.hlsli"

cbuffer TextureIndices : register(b1)
{
    uint pressureTexIndex;
}

cbuffer SimulationParam : register(b2)
{
    float2 pos;
    float2 posDelta;
    float4 splatColor;
    float2 simTexelSize;
    float colorDissipationSpeed;
    float velocityDissipationSpeed;
    float pressureDissipationSpeed;
    float curlIntensity;
    float aspectRatio;
    float splatRadius;
}

static Texture2D<float> pressureTex = ResourceDescriptorHeap[pressureTexIndex];

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float decay = 1.0 + pressureDissipationSpeed;
    return float4(pressureTex.Sample(pointClampSampler, texCoord) / decay, 0.0, 0.0, 1.0);
}