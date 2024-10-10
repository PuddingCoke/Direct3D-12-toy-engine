#include"Common.hlsli"
#include"Utility.hlsli"

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
    uint colorReadTexIndex;
    uint colorWriteTexIndex;
}

static Texture2D<float4> colorReadTex = ResourceDescriptorHeap[colorReadTexIndex];

static RWTexture2D<float4> colorWriteTex = ResourceDescriptorHeap[colorWriteTexIndex];

[numthreads(16, 9, 1)]
void main(const uint2 DTid : SV_DispatchThreadID)
{
    uint width, height;
    
    colorWriteTex.GetDimensions(width, height);
    
    //recalculate radius and position here
    const float newObstacleRadius = obstacleRadius * simTexelSize.x / perframeResource.screenTexelSize.x;
    
    const float2 newObstaclePosition = obstaclePosition * simTexelSize.x / perframeResource.screenTexelSize.x;
    
    const float2 dir = float2(DTid) - newObstaclePosition;
    
    //boundary and obstacle condition
    if (DTid.x == 0 || DTid.x == width - 1 || DTid.y == 0 || DTid.y == height - 1 /*|| length(dir) < newObstacleRadius*/)
    {
        colorWriteTex[DTid] = float4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        float2 texCoord = (float2(DTid) + float2(0.5, 0.5)) / float2(width, height);
        float2 p = texCoord - pos;
        p.x *= aspectRatio;
        float3 color = exp(-dot(p, p) / splatRadius) * splatColor.rgb * 0.15;
        float3 curColor = colorReadTex[DTid].rgb;
        colorWriteTex[DTid] = float4(color + curColor, 1.0);
    }
}