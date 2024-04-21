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

struct PixelOutput
{
    float4 color : SV_Target0;
    float dist : SV_Target1;
};

cbuffer TextureIndex : register(b2)
{
    uint diffuseTexIndex;
    uint specularTexIndex;
    uint normalTexIndex;
    uint shadowTexIndex;
    uint irradianceVolumeBufIndex;
};

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

float CalShadow(float3 P)
{
    float4 shadowPos = mul(float4(P, 1.0), volume.lightViewProj);
    
    shadowPos.xy = shadowPos.xy * float2(0.5, -0.5) + 0.5;
    
    return 1.0 - shadowTexture.SampleCmpLevelZero(shadowSampler, shadowPos.xy, shadowPos.z);
}

PixelOutput main(PixelInput input)
{
    const float4 baseColor = tDiffuse.Sample(linearWrapSampler, input.uv);
    
    if (baseColor.a < 0.9)
    {
        discard;
    }
    
    const float specular = tSpecular.Sample(linearWrapSampler, input.uv).r;
    
    const float3 V = normalize(probeLocation - input.pos);
    
    const float NdotL = max(dot(input.normal, volume.lightDir.xyz), 0.0);
    const float3 diffuseColor = volume.lightColor.rgb * baseColor.rgb * NdotL;
        
    const float3 H = normalize(V + volume.lightDir.xyz);
    const float NdotH = max(dot(input.normal, H), 0.0);
    const float3 specularColor = volume.lightColor.rgb * specular * pow(NdotH, 32.0);
   
    float shadow = CalShadow(input.pos);
    
    float dist = length(probeLocation - input.pos);
    
    float4 color = float4((diffuseColor + specularColor) * CalShadow(input.pos), 1.0);
    
    PixelOutput output;
    output.color = color;
    output.dist = dist;
    
    return output;
}