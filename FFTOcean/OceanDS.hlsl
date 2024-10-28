#include"Common.hlsli"

struct DS_OUTPUT
{
    float3 displacedPosition : POSITION0;
    float3 unDisplacedPosition : POSITION1;
    float3 viewDir : VIEWDIR;
    float2 uv : TEXCOORD;
    float4 svPosition : SV_POSITION;
};

struct HS_CONTROL_POINT_OUTPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[4] : SV_TessFactor;
    float InsideTessFactor[2] : SV_InsideTessFactor;
};

cbuffer TextureIndices : register(b2)
{
    uint displacementTextureIndex;
}

static Texture2D<float4> displacementTexture = ResourceDescriptorHeap[displacementTextureIndex];

float3 interpolatePosition(const float3 v0, const float3 v1, const float3 v2, const float3 v3, const float2 uv)
{
    const float3 bottom = lerp(v0, v1, uv.x);
    
    const float3 top = lerp(v3, v2, uv.x);
    
    const float3 p = lerp(bottom, top, uv.y);
    
    return p;
}

float2 interpolateUV(const float2 v0, const float2 v1, const float2 v2, const float2 v3, const float2 uv)
{
    const float2 bottom = lerp(v0, v1, uv.x);
    
    const float2 top = lerp(v3, v2, uv.x);
    
    const float2 p = lerp(bottom, top, uv.y);
    
    return p;
}

[domain("quad")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float2 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, 4> patch)
{
    DS_OUTPUT Output;

    float3 position = interpolatePosition(patch[0].position, patch[1].position, patch[2].position, patch[3].position, domain);
    
    const float2 uv = interpolateUV(patch[0].uv, patch[1].uv, patch[2].uv, patch[3].uv, domain);
    
    Output.unDisplacedPosition = position;
    
    Output.viewDir = perframeResource.eyePos.xyz - Output.unDisplacedPosition;
    
    position += displacementTexture.SampleLevel(linearWrapSampler, uv, 0.0).xyz;
    
    Output.displacedPosition = position;
    
    Output.uv = uv;
    
    Output.svPosition = mul(float4(position, 1.0), perframeResource.viewProj);

    return Output;
}
