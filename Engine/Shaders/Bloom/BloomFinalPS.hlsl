#include"Common.hlsli"

cbuffer BloomParam : register(b2)
{
    uint hdrTextureIdx;
    uint bloomTextureIdx;
    uint lensDirtTextureIdx;
    float exposure;
    float gamma;
    float threshold;
    float intensity;
}

static Texture2D<float4> hdrTexture = ResourceDescriptorHeap[hdrTextureIdx];

static Texture2D<float4> bloomTexture = ResourceDescriptorHeap[bloomTextureIdx];

static Texture2D<float4> lensDirtTexture = ResourceDescriptorHeap[lensDirtTextureIdx];

float3 ACESToneMapping(float3 color)
{
    const float A = 2.51;
    
    const float B = 0.03;
    
    const float C = 2.43;
    
    const float D = 0.59;
    
    const float E = 0.14;

    color *= exposure;
    
    return (color * (A * color + B)) / (color * (C * color + D) + E);
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float3 originColor = hdrTexture.Sample(linearClampSampler, texCoord).rgb;
    
    float3 dirtColor = lensDirtTexture.Sample(linearClampSampler, texCoord).rgb;
    
    float3 bloomColor = bloomTexture.Sample(linearClampSampler, texCoord).rgb * (1.0 + dirtColor);
    
    float3 result = lerp(originColor, originColor + bloomColor, intensity);
    
    result = ACESToneMapping(result);
    
    result = pow(result, 1.0 / gamma);
    
    return float4(result, 1.0);
}