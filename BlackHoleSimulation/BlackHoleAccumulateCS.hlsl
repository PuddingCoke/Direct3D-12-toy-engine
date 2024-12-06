#include"Common.hlsli"

#define FOVANGLE 80
#define MAXITERATION 300

cbuffer RenderParam : register(b2)
{
    uint accumulateTextureIndex;
    uint envCubeIndex;
    uint frameIndex;
    float phi;
    float theta;
    float radius;
}

static RWTexture2D<float4> accumulateTexture = ResourceDescriptorHeap[accumulateTextureIndex];

static TextureCube<float4> envCube = ResourceDescriptorHeap[envCubeIndex];

[numthreads(16, 9, 1)]
void main(const uint2 DTid : SV_DispatchThreadID)
{
    float2 uv = (float2(DTid) + float2(0.5, 0.5)) * perframeResource.screenTexelSize;
    
    uv.y = 1.0 - uv.y;
    
    uv = uv * 2.0 - 1.0;
    
    uv.x *= 16.0 / 9.0;
    
    const float3 cameraPos = float3(cos(phi) * cos(theta), cos(phi) * sin(theta), sin(phi)) * radius;
    
    const float3 helper = float3(0.0, 0.0, 1.0);
    
    const float3 xVec = normalize(cross(helper, cameraPos));
    
    const float3 yVec = normalize(cross(cameraPos, xVec));
    
    const float3 rayOrigin = cameraPos;
    
    const float3 rayDir = normalize(xVec * uv.x + yVec * uv.y - normalize(cameraPos) * 16.0 / 9.0 / tan(radians(FOVANGLE / 2.0)));
    
    //for (uint i = 0; i < MAXITERATION; i++)
    //{
    //    rad
    //}
    
    float3 color = envCube.SampleLevel(linearClampSampler, rayDir, 0.0).rgb;
    
    accumulateTexture[DTid] = lerp(accumulateTexture[DTid], float4(color, 1.0), 0.9);
}