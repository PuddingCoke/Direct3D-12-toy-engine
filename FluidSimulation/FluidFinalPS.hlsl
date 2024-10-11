#include"Common.hlsli"

cbuffer SimulationParam : register(b1)
{
    float2 pos;
    float2 posDelta;
    float4 splatColor;
    float2 colorTexelSize;
    float2 simTexelSize;
    uint2 colorTextureSize;
    uint2 simTextureSize;
    float colorDissipationSpeed;
    float velocityDissipationSpeed;
    float curlIntensity;
    float splatRadius;
}

cbuffer TextureIndices : register(b2)
{
    uint colorTexIndex;
}

static Texture2D<float4> colorTex = ResourceDescriptorHeap[colorTexIndex];

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float3 color = colorTex.Sample(linearClampSampler, texCoord).rgb;
    
    float3 lc = colorTex.Sample(linearClampSampler, texCoord - float2(perframeResource.screenTexelSize.x, 0.0)).rgb;
    
    float3 rc = colorTex.Sample(linearClampSampler, texCoord + float2(perframeResource.screenTexelSize.x, 0.0)).rgb;
    
    float3 tc = colorTex.Sample(linearClampSampler, texCoord + float2(0.0, perframeResource.screenTexelSize.y)).rgb;
    
    float3 bc = colorTex.Sample(linearClampSampler, texCoord - float2(0.0, perframeResource.screenTexelSize.y)).rgb;
    
    float dx = length(rc) - length(lc);

    float dy = length(tc) - length(bc);
    
    float3 n = normalize(float3(dx, dy, length(colorTexelSize)));
    
    float3 l = float3(0.0, 0.0, 1.0);
    
    float diffuse = clamp(dot(n, l) + 0.7, 0.7, 1.0);
    
    color *= diffuse;
    
    return float4(color, 1.0);
}