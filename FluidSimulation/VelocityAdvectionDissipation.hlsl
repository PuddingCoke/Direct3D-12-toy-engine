#include"Common.hlsli"

cbuffer TextureIndices : register(b2)
{
    uint velocityTexIndex;
}

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

static Texture2D<float2> velocityTex = ResourceDescriptorHeap[velocityTexIndex];

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float2 coord = texCoord - perframeResource.deltaTime * velocityTex.Sample(linearClampSampler, texCoord) * simTexelSize;
    const float2 result = velocityTex.Sample(linearClampSampler, coord);
    const float decay = 1.0 + velocityDissipationSpeed * perframeResource.deltaTime;
    return float4(result / decay, 0.0, 1.0);
}