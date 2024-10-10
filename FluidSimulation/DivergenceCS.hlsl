cbuffer TextureIndices : register(b2)
{
    uint velocityTexIndex;
    uint divergenceTexIndex;
};

static Texture2D<float2> velocityTex = ResourceDescriptorHeap[velocityTexIndex];

static RWTexture2D<float> divergenceTex = ResourceDescriptorHeap[divergenceTexIndex];

float DivergenceAt(const uint2 loc)
{
    const float uR = velocityTex[loc + uint2(1, 0)].x;
        
    const float uL = velocityTex[loc - uint2(1, 0)].x;
        
    const float vT = velocityTex[loc + uint2(0, 1)].y;
        
    const float vB = velocityTex[loc - uint2(0, 1)].y;
        
    const float divergence = 0.5 * (uR - uL + vT - vB);
    
    return divergence;
}

[numthreads(16, 9, 1)]
void main(const uint2 DTid : SV_DispatchThreadID)
{
    uint width, height;
    
    velocityTex.GetDimensions(width, height);
    
    //interior texel
    if (DTid.x > 0 && DTid.x < width - 1 && DTid.y > 0 && DTid.y < height - 1)
    {
        divergenceTex[DTid] = DivergenceAt(DTid);
    }
    //row texel
    else if (DTid.x > 0 && DTid.x < width - 1)
    {
        if (DTid.y == 0)
        {
            divergenceTex[DTid] = DivergenceAt(uint2(DTid.x, 1));
        }
        else if (DTid.y == height - 1)
        {
            divergenceTex[DTid] = DivergenceAt(uint2(DTid.x, height - 2));
        }
    }
    //column texel
    else if (DTid.y > 0 && DTid.y < height - 1)
    {
        if (DTid.x == 0)
        {
            divergenceTex[DTid] = DivergenceAt(uint2(1, DTid.y));
        }
        else if (DTid.x == width - 1)
        {
            divergenceTex[DTid] = DivergenceAt(uint2(width - 2, DTid.y));
        }
    }
    //corner texel
    else if (DTid.y == 0)
    {
        if (DTid.x == 0)
        {
            divergenceTex[DTid] = DivergenceAt(uint2(1, 1));
        }
        else if (DTid.x == width - 1)
        {
            divergenceTex[DTid] = DivergenceAt(uint2(width - 2, 1));
        }
    }
    else if (DTid.y == height - 1)
    {
        if (DTid.x == 0)
        {
            divergenceTex[DTid] = DivergenceAt(uint2(1, height - 2));
        }
        else if (DTid.x == width - 1)
        {
            divergenceTex[DTid] = DivergenceAt(uint2(width - 2, height - 2));
        }
    }
}