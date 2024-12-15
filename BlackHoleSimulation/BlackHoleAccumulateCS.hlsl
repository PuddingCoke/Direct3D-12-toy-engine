#include"Common.hlsli"

#define FOVANGLE 80
#define MAXITERATION 300

cbuffer RenderParam : register(b2)
{
    uint accumulateTextureIndex;
    uint envCubeIndex;
    uint noiseTextureIndex;
    uint diskTextureIndex;
    uint frameIndex;
    float phi;
    float theta;
    float radius;
}

static RWTexture2D<float4> accumulateTexture = ResourceDescriptorHeap[accumulateTextureIndex];

static TextureCube<float4> envCube = ResourceDescriptorHeap[envCubeIndex];

static Texture2D<float4> noiseTexture = ResourceDescriptorHeap[noiseTextureIndex];

static Texture2D<float4> diskTexture = ResourceDescriptorHeap[diskTextureIndex];

static const float3 blackHolePosition = float3(0.0, 0.0, 0.0);

static const float blackHoleMass = 0.5;

static const float eventHorizonRadius = 2.0 * blackHoleMass;

static const float photonSphereRadius = 3.0 * blackHoleMass;

static const float stepSize = 0.08;

[numthreads(16, 9, 1)]
void main(const uint2 DTid : SV_DispatchThreadID)
{
    float2 uv = (float2(DTid) + float2(0.5, 0.5)) * perframeResource.screenTexelSize;
    
    uv.y = 1.0 - uv.y;
    
    uv = uv * 2.0 - 1.0;
    
    uv.x *= 16.0 / 9.0;
    
    //we create a basic look at model here 
    //and use z axis as up vector of our coordinate system
    
    const float3 cameraPos = float3(cos(phi) * cos(theta), cos(phi) * sin(theta), sin(phi)) * radius;
    
    const float3 helper = float3(0.0, 0.0, 1.0);
    
    const float3 xVec = normalize(cross(helper, cameraPos));
    
    const float3 yVec = normalize(cross(cameraPos, xVec));
    
    float3 rayPos = cameraPos;
    
    float3 rayDir = normalize(xVec * uv.x + yVec * uv.y - normalize(cameraPos) * 16.0 / 9.0 / tan(radians(FOVANGLE / 2.0)));
    
    bool captured = false;
    
    float3 color = float3(0.0, 0.0, 0.0);
    
    float3 accumulatedGlowing = float3(0.0, 0.0, 0.0);
    
    float3 accumulatedDisk = float3(0.0, 0.0, 0.0);
    
    [loop]
    for (uint i = 0; i < MAXITERATION; i++)
    {
        const float3 toRayPos = rayPos - blackHolePosition;
        
        const float radiusSqr = dot(toRayPos, toRayPos);
        
        const float3 gravityVec = normalize(-toRayPos) * blackHoleMass / radiusSqr;
        
        rayDir = normalize(rayDir + stepSize * gravityVec);
        
        rayPos += rayDir * stepSize;
        
        const float radius = length(toRayPos);
        
        if (radius < eventHorizonRadius)
        {
            accumulatedGlowing = float3(0.0, 0.0, 0.0);
            
            captured = true;
            
            break;
        }
        
        accumulatedGlowing += float3(1.0, 1.0, 0.8) / (radiusSqr) * step(1.0, radius) * stepSize * 0.1;
        
        float2 polar = float2(length(rayPos.xy), atan2(rayPos.y, rayPos.x) + perframeResource.timeElapsed * 0.2);
        
        float3 diskColor = float3(1.0, 1.0, 0.8) * noiseTexture.SampleLevel(linearWrapSampler, polar, 0.0).r * stepSize;
        
        accumulatedDisk += diskColor * smoothstep(0.2, 0.0, abs(rayPos.z)) * step(1.5, radius) * step(radius, 3.5);
    }
    
    color += accumulatedGlowing;
    
    color += accumulatedDisk;
    
    if (!captured)
    {
        //color += envCube.SampleLevel(linearClampSampler, rayDir, 0.0).rgb;
    }
    
    accumulateTexture[DTid] = lerp(accumulateTexture[DTid], float4(color, 1.0), 0.9);
}