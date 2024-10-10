#include"Common.hlsli"

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

cbuffer TextureIndices : register(b2)
{
    uint velocityTexIndex;
    uint colorReadTexIndex;
    uint colorWriteTexIndex;
}

static Texture2D<float2> velocityTex = ResourceDescriptorHeap[velocityTexIndex];

static Texture2D<float4> colorReadTex = ResourceDescriptorHeap[colorReadTexIndex];

static RWTexture2D<float4> colorWriteTex = ResourceDescriptorHeap[colorWriteTexIndex];

[numthreads(16, 9, 1)]
void main(const uint2 DTid : SV_DispatchThreadID)
{
    uint width, height;
    
    colorWriteTex.GetDimensions(width, height);
    
    //boundary and obstacle condition
    if (DTid.x == 0 || DTid.x == width - 1 || DTid.y == 0 || DTid.y == height - 1)
    {
        colorWriteTex[DTid] = float4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        float2 texCoord = (float2(DTid) + float2(0.5, 0.5)) / float2(width, height);
        
        float2 coord = texCoord - perframeResource.deltaTime * velocityTex.SampleLevel(linearClampSampler, texCoord, 0.0) * simTexelSize;
        
        float3 result = colorReadTex.SampleLevel(linearClampSampler, coord, 0.0).rgb;
        
        float decay = 1.0 + colorDissipationSpeed * perframeResource.deltaTime;
        
        colorWriteTex[DTid] = float4(result / decay, 1.0);
    }
}