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
    float vorticityIntensity;
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
    float2 texCoord = (float2(loc) + float2(0.5, 0.5)) * colorTexelSize;
        
    texCoord -= pos;
    
    const float aspectRatio = 16.0 / 9.0;
    
    texCoord.x *= aspectRatio;
    
    const float3 color = exp(-dot(texCoord, texCoord) / splatRadius) * splatColor.rgb * 0.15;
        
    const float3 curColor = colorReadTex[loc].rgb;
    
    return color + curColor;
}

[numthreads(16, 9, 1)]
void main(const uint2 DTid : SV_DispatchThreadID)
{
    const float scale = simTexelSize.x / colorTexelSize.x;
    
    const float scaledRadius = obstacleRadius * scale;
    
    const float2 scaledPosition = obstaclePosition * scale;
    
    const float2 dir = (float2(DTid) + float2(0.5, 0.5)) - scaledPosition;
    
    if (DTid.x == 0 || DTid.x == colorTextureSize.x - 1 || DTid.y == 0 || DTid.y == colorTextureSize.y - 1 || length(dir) < scaledRadius)
    {
        colorWriteTex[DTid] = float4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        colorWriteTex[DTid] = float4(ColorAt(DTid), 1.0);
    }
}