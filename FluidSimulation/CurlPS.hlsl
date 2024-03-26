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
    const float L = velocityTex.Sample(linearClampSampler, texCoord - float2(simTexelSize.x, 0.0)).y;
    const float R = velocityTex.Sample(linearClampSampler, texCoord + float2(simTexelSize.x, 0.0)).y;
    const float T = velocityTex.Sample(linearClampSampler, texCoord + float2(0.0, simTexelSize.y)).x;
    const float B = velocityTex.Sample(linearClampSampler, texCoord - float2(0.0, simTexelSize.y)).x;
    const float vorticity = 0.5 * (T - B + L - R);
    return float4(vorticity, 0.0, 0.0, 1.0);
}