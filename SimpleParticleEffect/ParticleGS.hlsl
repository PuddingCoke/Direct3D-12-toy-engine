#include"Common.hlsli"

struct GeometryInput
{
    float4 color : COLOR;
    float4 position : SV_POSITION;
};

struct GeometryOutput
{
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float4 position : SV_POSITION;
};

cbuffer ParticleParam : register(b2)
{
    float particleSize;
};

static float3 billboardPos[4] =
{
    float3(-1, 1, 0),
    float3(1, 1, 0),
    float3(-1, -1, 0),
    float3(1, -1, 0),
};
    
static float2 billboardUV[4] =
{
    float2(0, 0),
    float2(1, 0),
    float2(0, 1),
    float2(1, 1),
};

[maxvertexcount(4)]
void main(
	point GeometryInput inputs[1],
	inout TriangleStream<GeometryOutput> outputs
)
{
    GeometryOutput output;
    
    [unroll]
    for (uint i = 0; i < 4; i++)
    {
        float3 position = billboardPos[i] * particleSize;
        
        position = mul(position, transpose((float3x3) perframeResource.normalMatrix)) + inputs[0].position.xyz;
        
        output.color = inputs[0].color;
        
        output.uv = billboardUV[i];
        
        output.position = mul(float4(position, 1.0), perframeResource.viewProj);

        outputs.Append(output);
    }
    
    outputs.RestartStrip();
}