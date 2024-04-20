#include"Common.hlsli"

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
    float4 position : SV_Target0;
    float4 normalSpecular : SV_Target1;
    float4 baseColor : SV_Target2;
};

cbuffer TextureIndex : register(b2)
{
    uint diffuseTexIndex;
    uint specularTexIndex;
    uint normalTexIndex;
}

static Texture2D tDiffuse = ResourceDescriptorHeap[diffuseTexIndex];

static Texture2D tSpecular = ResourceDescriptorHeap[specularTexIndex];

static Texture2D tNormal = ResourceDescriptorHeap[normalTexIndex];

PixelOutput main(PixelInput input)
{
    float4 baseColor = tDiffuse.Sample(linearWrapSampler, input.uv);
    
    if (baseColor.a < 0.9)
    {
        discard;
    }
    
    float3 N = normalize(input.normal);
    float3 B = normalize(input.binormal);
    float3 T = normalize(input.tangent);
    float3x3 TBN = float3x3(T, B, N);
    
    float specular = tSpecular.Sample(linearWrapSampler, input.uv).r;
    
    PixelOutput output;
    output.position = float4(input.pos, 1.0);
    output.normalSpecular = float4(mul(normalize(tNormal.Sample(linearWrapSampler, input.uv).xyz * 2.0 - 1.0), TBN), specular);
    output.baseColor = baseColor;
    
    return output;
}