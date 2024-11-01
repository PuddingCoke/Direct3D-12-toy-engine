#include"Common.hlsli"

cbuffer TextureIndices : register(b2)
{
    uint enviromentCubeIndex;
}

static TextureCube<float4> enviromentCube = ResourceDescriptorHeap[enviromentCubeIndex];

static const float3 L = normalize(float3(0.0, 1.0, -10.0));

float3 radiance(float3 viewDir, float3 sunDir, float turbidity)
{
    // Constants for the Preetham model
    const float A = 0.1787f * turbidity - 1.4630f;
    const float B = -0.3554f * turbidity + 0.4275f;
    const float C = -0.0227f * turbidity + 5.3251f;
    const float D = 0.1206f * turbidity - 2.5771f;
    const float E = -0.0670f * turbidity + 0.3703f;

    // Calculate the cosine of the angle between the view direction and the sun direction
    float cosTheta = dot(normalize(viewDir), normalize(sunDir));

    // Calculate the luminance
    float3 luminance = (1.0f + A * exp(B / (cosTheta + 0.01f))) * (1.0f + C * exp(D * cosTheta) + E * cosTheta * cosTheta);

    return luminance;
}

float4 main(const float3 position : POSITION) : SV_TARGET
{
    const float3 color = radiance(normalize(position), L, 2.0);
    
    return float4(color, 1.0);
    
    //const float3 color = 1.5 * enviromentCube.Sample(linearWrapSampler, position).rgb;
    
    //return float4(color, 1.0);
}