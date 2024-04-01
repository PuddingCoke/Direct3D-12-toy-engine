#include"Common.hlsli"

cbuffer TextureIndices : register(b2)
{
    uint velocityTexIndex;
    uint curlTexIndex;
}

cbuffer SimulationParam : register(b1)
{
    float2 pos;
    float2 posDelta;
    float4 splatColor;
    float2 simTexelSize;
    float colorDissipationSpeed;
    float velocityDissipationSpeed;
    float pressureDissipationSpeed;
    float curlIntensity;
    float aspectRatio;
    float splatRadius;
}

static Texture2D<float2> velocityTex = ResourceDescriptorHeap[velocityTexIndex];

static Texture2D<float> curlTex = ResourceDescriptorHeap[curlTexIndex];

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float L = curlTex.Sample(pointClampSampler, texCoord - float2(simTexelSize.x, 0.0));
    const float R = curlTex.Sample(pointClampSampler, texCoord + float2(simTexelSize.x, 0.0));
    const float T = curlTex.Sample(pointClampSampler, texCoord + float2(0.0, simTexelSize.y));
    const float B = curlTex.Sample(pointClampSampler, texCoord - float2(0.0, simTexelSize.y));
    const float C = curlTex.Sample(pointClampSampler, texCoord);
    
    float2 force = 0.5 * float2(abs(B) - abs(T), abs(R) - abs(L));
    force = force / (length(force) + 1e-5) * curlIntensity * C;
    
    const float2 curVel = velocityTex.Sample(linearClampSampler, texCoord);
    
    return float4(curVel + force * perframeResource.deltaTime, 0.0, 1.0);
}