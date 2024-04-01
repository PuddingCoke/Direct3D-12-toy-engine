#include"Common.hlsli"

cbuffer TextureIndices : register(b2)
{
    uint velocityTexIndex;
};

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

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float2 vL = texCoord - float2(simTexelSize.x, 0.0);
    const float2 vR = texCoord + float2(simTexelSize.x, 0.0);
    const float2 vT = texCoord + float2(0.0, simTexelSize.y);
    const float2 vB = texCoord - float2(0.0, simTexelSize.y);
    
    float L = velocityTex.Sample(linearClampSampler, texCoord - float2(simTexelSize.x, 0.0)).x;
    float R = velocityTex.Sample(linearClampSampler, texCoord + float2(simTexelSize.x, 0.0)).x;
    float T = velocityTex.Sample(linearClampSampler, texCoord + float2(0.0, simTexelSize.y)).y;
    float B = velocityTex.Sample(linearClampSampler, texCoord - float2(0.0, simTexelSize.y)).y;
    float2 C = velocityTex.Sample(linearClampSampler, texCoord).xy;
    
   //进行边界处理，NVIDIA的文章里讲了边界条件的散度必须为0
    if (vL.x < 0.0)
    {
        L = -C.x;
    }
    
    if (vR.x > 1.0)
    {
        R = -C.x;
    }
    
    if (vT.y > 1.0)
    {
        T = -C.y;
    }
    
    if (vB.y < 0.0)
    {
        B = -C.y;
    }
    
    float divergence = 0.5 * (R - L + T - B);
    
    return float4(divergence, 0.0, 0.0, 1.0);
}