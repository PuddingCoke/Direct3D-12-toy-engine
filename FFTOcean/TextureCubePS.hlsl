#include"Common.hlsli"

cbuffer TextureIndices : register(b2)
{
    uint enviromentCubeIndex;
}

static TextureCube<float4> enviromentCube = ResourceDescriptorHeap[enviromentCubeIndex];

float4 main(const float3 position : POSITION) : SV_TARGET
{
    const float3 color = enviromentCube.Sample(linearWrapSampler, position).rgb;
    
    return float4(color, 1.0);
}