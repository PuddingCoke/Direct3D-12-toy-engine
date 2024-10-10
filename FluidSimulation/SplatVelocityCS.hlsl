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

cbuffer TextureIndices : register(b2)
{
    uint velocityReadTexIndex;
    uint velocityWriteTexIndex;
}

static Texture2D<float2> velocityReadTex = ResourceDescriptorHeap[velocityReadTexIndex];

static RWTexture2D<float2> velocityWriteTex = ResourceDescriptorHeap[velocityWriteTexIndex];

[numthreads(16, 9, 1)]
void main(const uint2 DTid : SV_DispatchThreadID)
{
    uint width, height;
    
    velocityWriteTex.GetDimensions(width, height);
    
    bool isInteriorTexel = false;
    
    if (DTid.x > 0 && DTid.x < width - 1 && DTid.y > 0 && DTid.y < height - 1)
    {
        isInteriorTexel = true;
        
        float2 texCoord = (float2(DTid) + float2(0.5, 0.5)) / float2(width, height);
        float2 p = texCoord - pos;
        p.x *= aspectRatio;
        float2 velocity = exp(-dot(p, p) / splatRadius) * posDelta;
        float2 curVelocity = velocityReadTex[DTid];
        velocityWriteTex[DTid] = curVelocity + velocity;
    }
    
    AllMemoryBarrierWithGroupSync();
    
    //solve obstacle condition later because pressure gradient subtraction and velocity advection also need this
    //so we just write another shader solve this problem at once
    if (!isInteriorTexel)
    {
        //row texel
        if (DTid.x > 0 && DTid.x < width - 1)
        {
            if (DTid.y == 0)
            {
                float2 velocity = velocityWriteTex[uint2(DTid.x, 1)];
                
                velocity.y = -velocity.y;
                
                velocityWriteTex[DTid] = velocity;
            }
            else if (DTid.y == height - 1)
            {
                float2 velocity = velocityWriteTex[uint2(DTid.x, height - 2)];
                
                velocity.y = -velocity.y;
                
                velocityWriteTex[DTid] = velocity;
            }
        }
        //column texel
        else if (DTid.y > 0 && DTid.y < height - 1)
        {
            if (DTid.x == 0)
            {
                float2 velocity = velocityWriteTex[uint2(1, DTid.y)];
                
                velocity.x = -velocity.x;
                
                velocityWriteTex[DTid] = velocity;
            }
            else if (DTid.x == width - 1)
            {
                float2 velocity = velocityWriteTex[uint2(width - 2, DTid.y)];
                
                velocity.x = -velocity.x;
                
                velocityWriteTex[DTid] = velocity;
            }
        }
        //corner texel
        else if ((DTid.x == 0 || DTid.x == width - 1) && (DTid.y == 0 || DTid.y == height - 1))
        {
            velocityWriteTex[DTid] = float2(0.0, 0.0);
        }
    }
}