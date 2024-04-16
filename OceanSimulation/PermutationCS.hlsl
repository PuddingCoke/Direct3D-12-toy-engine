cbuffer TextureIndex : register(b2)
{
    uint outputIndex;
}

static RWTexture2D<float2> output = ResourceDescriptorHeap[outputIndex];

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float sign = ((((DTid.x + DTid.y) & 1u) == 1) ? -1.0 : 1.0);
    float2 result = output[DTid.xy] * sign;
    output[DTid.xy] = result;
}