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
    uint velocityTexIndex;
    uint colorReadTexIndex;
    uint colorWriteTexIndex;
}

static Texture2D<float2> velocityTex = ResourceDescriptorHeap[velocityTexIndex];

static Texture2D<float4> colorReadTex = ResourceDescriptorHeap[colorReadTexIndex];

static RWTexture2D<float4> colorWriteTex = ResourceDescriptorHeap[colorWriteTexIndex];

float3 ColorAt(const uint2 loc)
{
    float2 texCoord = (float2(loc) + float2(0.5, 0.5)) * colorTexelSize;
    
    texCoord -= perframeResource.deltaTime * velocityTex.SampleLevel(linearClampSampler, texCoord, 0.0) * simTexelSize;
    
    const float3 color = colorReadTex.SampleLevel(linearClampSampler, texCoord, 0.0).rgb;
    
    const float decay = 1.0 + colorDissipationSpeed * perframeResource.deltaTime;
    
    return color / decay;
}

[numthreads(16, 9, 1)]
void main(const uint2 DTid : SV_DispatchThreadID)
{   
    if (DTid.x == 0 || DTid.x == colorTextureSize.x - 1 || DTid.y == 0 || DTid.y == colorTextureSize.y - 1)
    {
        colorWriteTex[DTid] = float4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        colorWriteTex[DTid] = float4(ColorAt(DTid), 1.0);
    }
}