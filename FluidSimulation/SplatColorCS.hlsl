#include"Common.hlsli"
#include"Utility.hlsli"

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
    uint colorReadTexIndex;
    uint colorWriteTexIndex;
}

static Texture2D<float4> colorReadTex = ResourceDescriptorHeap[colorReadTexIndex];

static RWTexture2D<float4> colorWriteTex = ResourceDescriptorHeap[colorWriteTexIndex];

float3 ColorAt(const uint2 loc)
{
    const float2 texCoord = (float2(loc) + float2(0.5, 0.5)) * colorTexelSize;
        
    float2 p = texCoord - pos;
    
    const float aspectRatio = colorTexelSize.y / colorTexelSize.x;
    
    p.x *= aspectRatio;
    
    const float3 color = exp(-dot(p, p) / splatRadius) * splatColor.rgb * 0.05;
        
    const float3 curColor = colorReadTex[loc].rgb;
    
    return color + curColor;
}

[numthreads(16, 9, 1)]
void main(const uint2 DTid : SV_DispatchThreadID)
{
    const float scale = simTexelSize.x / colorTexelSize.x;
    
    const float newObstacleRadius = obstacleRadius * scale;
    
    const float2 newObstaclePosition = obstaclePosition * scale;
    
    const float2 dir = (float2(DTid) + float2(0.5, 0.5)) - newObstaclePosition;
    
    if (DTid.x == 0 || DTid.x == colorTextureSize.x - 1 || DTid.y == 0 || DTid.y == colorTextureSize.y - 1 || length(dir) < newObstacleRadius)
    {
        colorWriteTex[DTid] = float4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        colorWriteTex[DTid] = float4(ColorAt(DTid), 1.0);
    }
}