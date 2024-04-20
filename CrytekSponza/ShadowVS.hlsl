#include"Utility.hlsli"

struct VertexInput
{
    float3 pos : POSITION;
};

ConstantBuffer<IrradianceVolume> volume : register(b3);

float4 main(VertexInput input) : SV_POSITION
{
    return mul(float4(input.pos, 1.0), volume.lightViewProj);
}