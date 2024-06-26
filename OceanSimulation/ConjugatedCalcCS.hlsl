cbuffer TextureIndex : register(b2)
{
    uint tildeh0Index;
    uint tildeh0kIndex;
}

static RWTexture2D<float4> tildeh0 = ResourceDescriptorHeap[tildeh0Index];

static Texture2D<float2> tildeh0k = ResourceDescriptorHeap[tildeh0kIndex];

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float2 h0K = tildeh0k[DTid.xy];
    float2 h0MinusK = tildeh0k[uint2((1024 - DTid.x) % 1024, (1024 - DTid.y) % 1024)];
    tildeh0[DTid.xy] = float4(h0K.x, h0K.y, h0MinusK.x, -h0MinusK.y);
}