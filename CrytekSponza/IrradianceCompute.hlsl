#include"Common.hlsli"

#define PI 3.141592653589793

struct Sample
{
    float4 direction;
    float Ylm[9];
};

cbuffer TextureIndex : register(b2)
{
    uint irradianceCoeffTexIndex;
    uint envCubeTexIndex;
    uint samplesBufIndex;
}

cbuffer ProjMatrices : register(b3)
{
    matrix viewProj[6];
    float3 probeLocation;
    uint probeIndex;
}

static RWTexture2DArray<float3> irradianceCoeff = ResourceDescriptorHeap[irradianceCoeffTexIndex];

static TextureCube envCube = ResourceDescriptorHeap[envCubeTexIndex];

static StructuredBuffer<Sample> samples = ResourceDescriptorHeap[samplesBufIndex];

[numthreads(9, 1, 1)]
void main(uint2 DTid : SV_DispatchThreadID)
{
    float3 result = float3(0.0, 0.0, 0.0);
    
    uint numSamples;
    
    uint stride;
    
    samples.GetDimensions(numSamples, stride);
    
    for (uint i = 0; i < numSamples; i++)
    {
        result += envCube.SampleLevel(linearWrapSampler, samples[i].direction.xyz, 0.0).rgb * samples[i].Ylm[DTid.x];
    }
    
    result *= 4.0 * PI / float(numSamples);

    irradianceCoeff[uint3(DTid, probeIndex)] = result;

}