#include"Common.hlsli"

cbuffer TextureIndices : register(b2)
{
    uint pressureTexIndex;
    uint divergenceTexIndex;
};

cbuffer SimulationParam : register(b1)
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

static Texture2D<float> divergenceTex = ResourceDescriptorHeap[divergenceTexIndex];

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float L = pressureTex.Sample(pointClampSampler, texCoord - float2(simTexelSize.x, 0.0));
    const float R = pressureTex.Sample(pointClampSampler, texCoord + float2(simTexelSize.x, 0.0));
    const float T = pressureTex.Sample(pointClampSampler, texCoord + float2(0.0, simTexelSize.y));
    const float B = pressureTex.Sample(pointClampSampler, texCoord - float2(0.0, simTexelSize.y));
    const float C = pressureTex.Sample(pointClampSampler, texCoord);
    const float divergence = divergenceTex.Sample(pointClampSampler, texCoord);
    const float pressure = (L + R + B + T - divergence) * 0.25;
    return float4(pressure, 0.0, 0.0, 1.0);
}