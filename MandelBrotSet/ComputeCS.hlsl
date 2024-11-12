cbuffer TextureIndices : register(b2)
{
    uint outputTextureIndex;
    float2 location;
    float scale;
    float2 texelSize;
    float dividen;
}

static RWTexture2D<float4> outputTexture = ResourceDescriptorHeap[outputTextureIndex];

#define MAXITERATION 500
#define ESCAPEBOUNDARY 16.0

float2 ComplexSqr(in const float2 c)
{
    return float2(c.x * c.x - c.y * c.y, 2.0 * c.x * c.y);
}

static const float values[] = { 0.0, 0.16, 0.42, 0.6425, 0.8575, 1.0 };

static const float3 colors[] =
{
    { 0, 7, 100 },
    { 32, 107, 203 },
    { 237, 255, 255 },
    { 255, 170, 0 },
    { 0, 2, 0 },
    { 0, 7, 100 },
};

static const float3 tangent[] =
{
    { 200, 625, 643.75 },
    { 494.231, 597.115, 421.875 },
    { 434.68, 93.6041, -473.034 },
    { -552.574, -581.709, 0 },
    { 0, -373.154, 0 },
    { 0, 35.0877, 701.754 }
};

float3 interpolateColor(float t)
{
    [unroll]
    for (uint i = 0; i < 6; i++)
    {
        if (values[i + 1] >= t)
        {
            t = (t - values[i]) / (values[i + 1] - values[i]);
            
            return (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * colors[i] +
					(pow(t, 3) - 2 * pow(t, 2) + t) * (values[i + 1] - values[i]) * tangent[i] +
					(-2 * pow(t, 3) + 3 * pow(t, 2)) * colors[i + 1] +
					(pow(t, 3) - pow(t, 2)) * (values[i + 1] - values[i]) * tangent[i + 1];
        }
    }

    return float3(0.0, 0.0, 0.0);
}

float2 complexMul(const float2 a, const float2 b)
{
    return float2(a.x * b.x - a.y * b.y, a.y * b.x + a.x * b.y);
}

[numthreads(16, 9, 1)]
void main(const uint2 DTid : SV_DispatchThreadID)
{
    const float2 originPosition = ((float2(DTid) + float2(0.5, 0.5)) * texelSize - 0.5) * float2(2.2 * 16.0 / 9.0, 2.2) * scale + location;
    
    float2 position = originPosition;
    
    const float2 c = float2(-0.5251993, -0.5251993);
    
    //const float2 c = originPosition;
    
    float2 dz = float2(1.0, 0.0);
    
    float2 dz_sum = float2(0.0, 0.0);
    
    uint i = 0;
    
    [unroll]
    for (; i < MAXITERATION; i++)
    {
        if (dot(position, position) > ESCAPEBOUNDARY)
        {
            break;
        }
        
        position = ComplexSqr(position) + c;
        
        //dz = 2.0 * complexMul(dz, position) + float2(1.0, 0.0);
        
        //dz_sum += dz;
    }
    
    //const float smoothed_i = float(i) - log2(max(1.0, log2(length(position))));
    
    //float iter_ratio = saturate(smoothed_i / float(MAXITERATION)) * 0.8575;
    
    const float smoothed_i = log2(log2(dot(position, position)) / 2.0);
    
    float colorI = sqrt(i + 10.0 - smoothed_i) / dividen*0.8575;
    
    const float3 color = interpolateColor(colorI) / 255.0;
    
    outputTexture[DTid] = float4(color, 1.0);
}