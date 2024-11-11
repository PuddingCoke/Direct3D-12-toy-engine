cbuffer TextureIndices : register(b2)
{
    uint outputTextureIndex;
    float2 location;
    float scale;
    float2 texelSize;
}

static RWTexture2D<float4> outputTexture = ResourceDescriptorHeap[outputTextureIndex];

#define MAXITERATION 500
#define MAXRADIUS 4.0

float2 ComplexSqr(const float2 c)
{
    return float2(c.x * c.x - c.y * c.y, 2.0 * c.x * c.y);;
}

static const float3 colors[8] =
{
    { 25.0, 24.0, 23.0 },
    { 120.0, 90.0, 70.0 },
    { 130.0, 24.0, 23.0 },
    { 250.0, 179.0, 100.0 },
    { 43.0, 65.0, 98.0 },
    { 11.0, 110.0, 79.0 },
    { 150.0, 110.0, 79.0 },
    { 255.0, 255.0, 255.0 }
};

static const float values[8] =
{
    0.0,
    0.03,
    0.05,
    0.25,
    0.5,
    0.85,
    0.95,
    1.0
};

float3 interpolateColor(float value)
{
    if (value >= 1.0)
    {
        return colors[7];
    }
    else if (value <= 0.0)
    {
        return colors[0];
    }
    
    [unroll]
    for (uint i = 0; i < 8; i++)
    {
        if (values[i] > value)
        {
            const float range = values[i] - values[i - 1];
            
            const float pos = value - values[i - 1];
            
            const float ratio = pos / range;
            
            return lerp(colors[i - 1], colors[i], ratio);
        }
    }
    
    return float3(255.0, 255.0, 255.0);
}

[numthreads(16, 9, 1)]
void main(const uint2 DTid : SV_DispatchThreadID)
{
    float2 originPosition = ((float2(DTid) + float2(0.5, 0.5)) * texelSize - 0.5) * float2(16.0 / 9.0, 1.0) * scale + location;
    
    float2 position = float2(0.0, 0.0);
    
    uint i = 0;
    
    [unroll]
    for (; i < MAXITERATION; i++)
    {
        position = ComplexSqr(position) + originPosition;
        
        if (dot(position, position) > MAXRADIUS)
        {
            break;
        }
    }
    
    outputTexture[DTid] = float4((float(i) / float(MAXITERATION)).rrr, 1.0);
}