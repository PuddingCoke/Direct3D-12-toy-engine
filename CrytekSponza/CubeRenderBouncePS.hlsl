#include"Common.hlsli"

#include"Utility.hlsli"

struct PixelInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

cbuffer TextureIndex : register(b2)
{
    uint diffuseTexIndex;
    uint specularTexIndex;
    uint normalTexIndex;
    uint shadowTexIndex;
    uint irradianceVolumeBufIndex;
    uint irradianceOctahedralMapTexIndex;
    uint depthOctahedralMapTexIndex;
}

cbuffer ProjMatrices : register(b3)
{
    matrix viewProj[6];
    float3 probeLocation;
    uint probeIndex;
}

static Texture2D tDiffuse = ResourceDescriptorHeap[diffuseTexIndex];

static Texture2D tSpecular = ResourceDescriptorHeap[specularTexIndex];

static Texture2D tNormal = ResourceDescriptorHeap[normalTexIndex];

static Texture2D<float> shadowTexture = ResourceDescriptorHeap[shadowTexIndex];

static ConstantBuffer<IrradianceVolume> volume = ResourceDescriptorHeap[irradianceVolumeBufIndex];

static Texture2DArray<float3> irradianceOctahedralMap = ResourceDescriptorHeap[irradianceOctahedralMapTexIndex];

static Texture2DArray<float2> depthOctahedralMap = ResourceDescriptorHeap[depthOctahedralMapTexIndex];

float CalShadow(float3 P)
{
    float4 shadowPos = mul(float4(P, 1.0), volume.lightViewProj);
    
    shadowPos.xy = shadowPos.xy * float2(0.5, -0.5) + 0.5;
    
    return 1.0 - shadowTexture.SampleCmpLevelZero(shadowSampler, shadowPos.xy, shadowPos.z);
}

float4 main(PixelInput input) : SV_Target
{
    const float4 baseColor = tDiffuse.Sample(linearWrapSampler, input.uv);
    
    clip(baseColor.a - 0.9);
    
    const float specular = tSpecular.Sample(linearWrapSampler, input.uv).r;
    
    const float3 V = normalize(probeLocation - input.pos);
    
    const float NdotL = max(dot(input.normal, volume.lightDir.xyz), 0.0);
    const float3 diffuseColor = volume.lightColor.rgb * baseColor.rgb * NdotL;
        
    const float3 H = normalize(V + volume.lightDir.xyz);
    const float NdotH = max(dot(input.normal, H), 0.0);
    const float3 specularColor = volume.lightColor.rgb * specular * pow(NdotH, 32.0);
   
    float shadow = CalShadow(input.pos);
    
    float3 color = (diffuseColor + specularColor) * CalShadow(input.pos);
    
    color += baseColor.rgb * GetIndirectDiffuse(input.pos, input.normal, volume, irradianceOctahedralMap, depthOctahedralMap, linearClampSampler);
    
    return float4(color, 1.0);
}