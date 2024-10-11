cbuffer SimulationParam : register(b1)
{
    float2 pos;
    float2 posDelta;
    float4 splatColor;
    float2 colorTexelSize;
    float2 simTexelSize;
    uint2 colorTextureSize;
    uint2 simTextureSize;
    float colorDissipationSpeed;
    float velocityDissipationSpeed;
    float vorticityIntensity;
    float splatRadius;
}

cbuffer TextureIndices : register(b2)
{
    uint colorTexIndex;
    uint originTexIndex;
}

static Texture2D<float4> colorTex = ResourceDescriptorHeap[colorTexIndex];

static RWTexture2D<float4> originTex = ResourceDescriptorHeap[originTexIndex];

[numthreads(16, 9, 1)]
void main(const uint2 DTid : SV_DispatchThreadID)
{
    if (DTid.x > 0 && DTid.x < colorTextureSize.x - 1 && DTid.y > 0 && DTid.y < colorTextureSize.y - 1)
    {
        float3 color = colorTex[DTid].rgb;
    
        const float3 R = colorTex[DTid + uint2(1, 0)].rgb;
                                                                                       
        const float3 L = colorTex[DTid - uint2(1, 0)].rgb;
                                                                                       
        const float3 T = colorTex[DTid + uint2(0, 1)].rgb;
                                                                                       
        const float3 B = colorTex[DTid - uint2(0, 1)].rgb;
    
        const float dx = length(R) - length(L);

        const float dy = length(T) - length(B);
    
        const float3 normal = normalize(float3(dx, dy, length(colorTexelSize)));
    
        const float3 light = float3(0.0, 0.0, 1.0);
    
        const float diffuse = clamp(dot(normal, light) + 0.7, 0.7, 1.0);
    
        color *= diffuse;
    
        originTex[DTid] = float4(color, 1.0);
    }
    else
    {
        originTex[DTid] = float4(0.0, 0.0, 0.0, 1.0);
    }
}