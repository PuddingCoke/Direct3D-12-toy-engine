#include"Common.hlsli"

cbuffer TextureIndices : register(b1)
{
    uint pressureTexIndex;
    uint velocityTexIndex;
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

static Texture2D<float2> velocityTex = ResourceDescriptorHeap[velocityTexIndex];

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float L = pressureTex.Sample(pointClampSampler, texCoord - float2(simTexelSize.x, 0.0));
    const float R = pressureTex.Sample(pointClampSampler, texCoord + float2(simTexelSize.x, 0.0));
    const float T = pressureTex.Sample(pointClampSampler, texCoord + float2(0.0, simTexelSize.y));
    const float B = pressureTex.Sample(pointClampSampler, texCoord - float2(0.0, simTexelSize.y));
    float2 velocity = velocityTex.Sample(linearClampSampler, texCoord);
    velocity.xy -= 0.5 * float2(R - L, T - B);
    return float4(velocity, 0.0, 1.0);
}