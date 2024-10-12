cbuffer TextureIndices : register(b2)
{
    uint pressureTexIndex;
}

static RWTexture2D<float> pressureTex = ResourceDescriptorHeap[pressureTexIndex];

[numthreads(16, 9, 1)]
void main(const uint2 DTid : SV_DispatchThreadID)
{
    pressureTex[DTid] = 0.0;
}