#include"Common.hlsli"

cbuffer TextureIndices : register(b1)
{
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

static Texture2D<float2> velocityTex = ResourceDescriptorHeap[velocityTexIndex];

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float2 p = texCoord - pos;
    p.x *= aspectRatio;
    float2 velocity = exp(-dot(p, p) / splatRadius) * posDelta;
    float2 curVelocity = velocityTex.Sample(linearClampSampler, texCoord);
    return float4(curVelocity + velocity, 0.0, 1.0);
}