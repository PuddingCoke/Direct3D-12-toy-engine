cbuffer TextureIndices : register(b2)
{
    uint divergenceTexIndex;
    uint pressureReadTexIndex;
    uint pressureWriteTexIndex;
};

static Texture2D<float> divergenceTex = ResourceDescriptorHeap[divergenceTexIndex];

static Texture2D<float> pressureReadTex = ResourceDescriptorHeap[pressureReadTexIndex];

static RWTexture2D<float> pressureWriteTex = ResourceDescriptorHeap[pressureWriteTexIndex];

float PressureAt(const uint2 loc)
{
    const float R = pressureReadTex[loc + uint2(1, 0)];
        
    const float L = pressureReadTex[loc - uint2(1, 0)];
        
    const float T = pressureReadTex[loc + uint2(0, 1)];
        
    const float B = pressureReadTex[loc - uint2(0, 1)];
        
    const float C = divergenceTex[loc];
    
    const float pressure = (R + L + T + B - C) * 0.25;
    
    return pressure;
}

[numthreads(16, 9, 1)]
void main(const uint2 DTid : SV_DispatchThreadID)
{
    uint width, height;
    
    pressureWriteTex.GetDimensions(width, height);
    
    //interior texel
    if (DTid.x > 0 && DTid.x < width - 1 && DTid.y > 0 && DTid.y < height - 1)
    {
        pressureWriteTex[DTid] = PressureAt(DTid);
    }
    //row texel
    else if (DTid.x > 0 && DTid.x < width - 1)
    {
        if (DTid.y == 0)
        {
            pressureWriteTex[DTid] = PressureAt(uint2(DTid.x, 1));
        }
        else if (DTid.y == height - 1)
        {
            pressureWriteTex[DTid] = PressureAt(uint2(DTid.x, height - 2));
        }
    }
    //column texel
    else if (DTid.y > 0 && DTid.y < height - 1)
    {
        if (DTid.x == 0)
        {
            pressureWriteTex[DTid] = PressureAt(uint2(1, DTid.y));
        }
        else if (DTid.x == width - 1)
        {
            pressureWriteTex[DTid] = PressureAt(uint2(width - 2, DTid.y));
        }
    }
    //corner texel
    else if (DTid.y == 0)
    {
        if (DTid.x == 0)
        {
            pressureWriteTex[DTid] = PressureAt(uint2(1, 1));
        }
        else if (DTid.x == width - 1)
        {
            pressureWriteTex[DTid] = PressureAt(uint2(width - 2, 1));
        }
    }
    else if (DTid.y == height - 1)
    {
        if (DTid.x == 0)
        {
            pressureWriteTex[DTid] = PressureAt(uint2(1, height - 2));
        }
        else if (DTid.x == width - 1)
        {
            pressureWriteTex[DTid] = PressureAt(uint2(width - 2, height - 2));
        }
    }
}