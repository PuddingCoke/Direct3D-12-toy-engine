#include"Common.hlsli"

struct PixelInput
{
    float2 texCoord : TEXCOORD;
    float4 lodScales : LODSCALES;
    float3 viewDir : VIEWDIR;
};

cbuffer TextureIndices : register(b2)
{
    uint displacement0TextureIndex;
    uint derivative0TextureIndex;
    uint jacobian0TextureIndex;
    uint displacement1TextureIndex;
    uint derivative1TextureIndex;
    uint jacobian1TextureIndex;
    uint displacement2TextureIndex;
    uint derivative2TextureIndex;
    uint jacobian2TextureIndex;
    uint enviromentCubeIndex;
}

cbuffer RenderParam : register(b3)
{
    float lodScale;
    float lengthScale0;
    float lengthScale1;
    float lengthScale2;
    float fogPower;
    float sunTheta;
    float specularPower;
}

static Texture2D<float4> displacement0Texture = ResourceDescriptorHeap[displacement0TextureIndex];
static Texture2D<float4> derivative0Texture = ResourceDescriptorHeap[derivative0TextureIndex];
static Texture2D<float> jacobian0Texture = ResourceDescriptorHeap[jacobian0TextureIndex];

static Texture2D<float4> displacement1Texture = ResourceDescriptorHeap[displacement1TextureIndex];
static Texture2D<float4> derivative1Texture = ResourceDescriptorHeap[derivative1TextureIndex];
static Texture2D<float> jacobian1Texture = ResourceDescriptorHeap[jacobian1TextureIndex];

static Texture2D<float4> displacement2Texture = ResourceDescriptorHeap[displacement2TextureIndex];
static Texture2D<float4> derivative2Texture = ResourceDescriptorHeap[derivative2TextureIndex];
static Texture2D<float> jacobian2Texture = ResourceDescriptorHeap[jacobian2TextureIndex];

static TextureCube<float4> enviromentCube = ResourceDescriptorHeap[enviromentCubeIndex];

static const float3 L = normalize(float3(0.0, sin(sunTheta), cos(sunTheta)));

static const float3 oceanColor = float3(0.0000, 0.2507, 0.3613);

static const float3 sunColor = float3(1.0, 1.0, 1.0);

float4 main(PixelInput input) : SV_TARGET
{
    float4 derivative = derivative0Texture.Sample(anisotrophicWrapSampler, input.texCoord / lengthScale0);
    
    derivative += derivative1Texture.Sample(anisotrophicWrapSampler, input.texCoord / lengthScale1) * input.lodScales.y;
    
    derivative += derivative2Texture.Sample(anisotrophicWrapSampler, input.texCoord / lengthScale2) * input.lodScales.z;
    
    float J = jacobian0Texture.Sample(anisotrophicWrapSampler, input.texCoord / lengthScale0);
    
    J += jacobian1Texture.Sample(anisotrophicWrapSampler, input.texCoord / lengthScale1);
    
    J += jacobian2Texture.Sample(anisotrophicWrapSampler, input.texCoord / lengthScale2);
    
    float2 slope = float2(derivative.x / (1.0 + derivative.z), derivative.y / (1.0 + derivative.w));
    
    float3 N = normalize(float3(-slope.x, 1.0, -slope.y));
    
    float3 V = normalize(input.viewDir);
    
    if (dot(N, V) < 0.0)
    {
        N = normalize(N - 2.0 * dot(N, V) * V);
    }
    
    float3 R = reflect(-V, N);
    
    float F0 = 0.020018673;
    
    float F = F0 + (1.0 - F0) * pow(1.0 - dot(N, V), 5.0);
    
    float3 H = normalize(V + L);
    
    float3 reflectColor = enviromentCube.Sample(linearWrapSampler, R).rgb;
    
    float turbulence = max(1.6 - J, 0.0);
    
    float highlightMul = 1.0 + 3.0 * smoothstep(1.2, 1.8, turbulence);
    
    float3 color = lerp(oceanColor, reflectColor * highlightMul, F) + pow(max(dot(L, R), 0.0), specularPower) * sunColor;
    
    float fogFactor = distance(perframeResource.eyePos.xz, input.texCoord);
    
    float2 fogTexcoord = normalize(input.texCoord - perframeResource.eyePos.xz);
    
    float3 fogColor = enviromentCube.Sample(linearWrapSampler, float3(fogTexcoord.x, 0.0, fogTexcoord.y)).rgb;
    
    fogFactor = pow(smoothstep(0.0, 4096.0, fogFactor), fogPower);
    
    color = lerp(color, fogColor, fogFactor);
    
    return float4(color, 1.0);
}