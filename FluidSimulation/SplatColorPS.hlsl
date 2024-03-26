#include"Common.hlsli"

cbuffer TextureIndices : register(b1)
{
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

static Texture2D<float4> colorTex = ResourceDescriptorHeap[colorTexIndex];

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float2 p = texCoord - pos;
    p.x *= aspectRatio;
    float3 color = exp(-dot(p, p) / splatRadius) * splatColor.rgb * 0.15;
    float3 curColor = colorTex.Sample(linearClampSampler, texCoord).rgb;
    return float4(curColor + color, 1.0);
}