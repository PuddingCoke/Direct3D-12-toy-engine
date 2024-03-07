struct Indices
{
    uint textureIdx;
};

ConstantBuffer<Indices> indicesData : register(b1);

SamplerState samplerState : register(s0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    Texture2D<float4> tex = ResourceDescriptorHeap[indicesData.textureIdx];
    
    float3 color = tex.Sample(samplerState, texCoord).rgb;
    
    color.rgb = pow(color.rgb, 1.0 / 2.2);
    
    return float4(color, 1.0);
}