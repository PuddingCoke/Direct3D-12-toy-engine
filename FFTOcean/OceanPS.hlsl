#include"Common.hlsli"

struct PixelInput
{
    float3 unDisplacedPosition : POSITION1;
    float3 viewDir : VIEWDIR;
    float2 uv : TEXCOORD;
};

cbuffer TextureIndices : register(b2)
{
    uint displacementTextureIndex;
    uint derivativeTextureIndex;
    uint jacobianTextureIndex;
    uint enviromentCubeIndex;
}

static Texture2D<float4> displacementTexture = ResourceDescriptorHeap[displacementTextureIndex];

static Texture2D<float4> derivativeTexture = ResourceDescriptorHeap[derivativeTextureIndex];

static Texture2D<float> jacobianTexture = ResourceDescriptorHeap[jacobianTextureIndex];

static TextureCube<float4> enviromentCube = ResourceDescriptorHeap[enviromentCubeIndex];

static const float3 L = normalize(float3(0.0, 1.0, -10.0));

static const float3 oceanColor = float3(0.0000, 0.2307, 0.3613);

static const float3 sunColor = float3(1.0, 1.0, 1.0);

float4 main(PixelInput input) : SV_TARGET
{
    float4 derivative = derivativeTexture.Sample(linearWrapSampler, input.uv);
    
    float2 slope = float2(derivative.x / (1.0 + derivative.z), derivative.y / (1.0 + derivative.w));
    
    float3 N = normalize(float3(-slope.x, 1.0, -slope.y));
    
    float3 V = normalize(input.viewDir);
    
    if (dot(N, V) < 0.0)
    {
        N = normalize(N - 2.0 * dot(N, V) * V);
    }
    
    float3 R = reflect(-V, N);
    
    float F0 = 0.020018673;
    
    float F = F0 + (1.0 - F0) * pow(1.0 - dot(N, V), 5.0);
    
    float3 H = normalize(V + L);
    
    float3 reflectColor = 1.5 * enviromentCube.Sample(linearWrapSampler, R).rgb;
    
    float J = jacobianTexture.Sample(linearWrapSampler, input.uv);
    
    float turbulence = max(1.6 - J, 0.0);
    
    float highlightMul = 1.0 + 3.0 * smoothstep(1.2, 1.8, turbulence);
    
    float3 color = lerp(oceanColor, reflectColor * highlightMul, F) + pow(max(dot(L, R), 0.0), 256.0) * sunColor;
    
    float fogFactor = distance(perframeResource.eyePos.xz, input.unDisplacedPosition.xz);
    
    float2 fogTexcoord = normalize(input.unDisplacedPosition.xz - perframeResource.eyePos.xz);
    
    float3 fogColor = enviromentCube.Sample(linearWrapSampler, float3(fogTexcoord.x, 0.0, fogTexcoord.y)).rgb;
    
    fogFactor = pow(smoothstep(0.0, 1600.0, fogFactor), 0.5);
    
    color = lerp(color, fogColor, fogFactor);
    
    return float4(color, 1.0);
}