#include"Common.hlsli"

cbuffer TextureIndices : register(b1)
{
    uint velocityTexIndex;
    uint colorTexIndex;
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

static Texture2D<float2> velocityTex = ResourceDescriptorHeap[velocityTexIndex];

static Texture2D<float4> colorTex = ResourceDescriptorHeap[colorTexIndex];

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float2 coord = texCoord - perframeResource.deltaTime * velocityTex.Sample(linearClampSampler, texCoord) * simTexelSize;
    float4 result = colorTex.Sample(linearClampSampler, coord);
    float decay = 1.0 + colorDissipationSpeed * perframeResource.deltaTime;
    return result / decay;
}