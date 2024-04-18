#include"Common.hlsli"

#define BASETESSFACTOR 8.0

#define FINALTESSFACTOR 4.0

struct VS_CONTROL_POINT_OUTPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
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

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, 4> ip,
	uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;
    
    const float3 centerPos = (ip[0].position + ip[1].position + ip[2].position + ip[3].position) / 4.0;
    
    const float dist = distance(perframeResource.eyePos.xyz, centerPos);
    
    float tessFactor = lerp(BASETESSFACTOR, FINALTESSFACTOR, saturate(dist / 512.0));
    
    Output.EdgeTessFactor[0] = tessFactor;
    Output.EdgeTessFactor[1] = tessFactor;
    Output.EdgeTessFactor[2] = tessFactor;
    Output.EdgeTessFactor[3] = tessFactor;
    Output.InsideTessFactor[0] = BASETESSFACTOR;
    Output.InsideTessFactor[1] = BASETESSFACTOR;

    return Output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main(
	InputPatch<VS_CONTROL_POINT_OUTPUT, 4> ip,
	uint i : SV_OutputControlPointID)
{
    HS_CONTROL_POINT_OUTPUT Output;
    Output.position = ip[i].position;
    Output.uv = ip[i].uv;
    return Output;
}
