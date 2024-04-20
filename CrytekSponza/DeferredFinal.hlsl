#include"Common.hlsli"

#include"Utility.hlsli"

cbuffer TextureIndex : register(b2)
{
    uint gPositionTexIndex;
    uint gNormalSpecularTexIndex;
    uint gBaseColorTexIndex;
    uint shadowTexIndex;
}

ConstantBuffer<IrradianceVolume> volume : register(b3);

static Texture2D gPosition = ResourceDescriptorHeap[gPositionTexIndex];

static Texture2D gNormalSpecular = ResourceDescriptorHeap[gNormalSpecularTexIndex];

static Texture2D gBaseColor = ResourceDescriptorHeap[gBaseColorTexIndex];

static Texture2D<float> shadowTexture = ResourceDescriptorHeap[shadowTexIndex];

float CalShadow(float3 P)
{
    float4 shadowPos = mul(float4(P, 1.0), volume.lightViewProj);
    shadowPos.xy = shadowPos.xy * float2(0.5, -0.5) + 0.5;
    
    float shadow = 0.0;
    const float2 texelSize = 1.0 / float2(4096.0, 4096.0);
    
    [unroll]
    for (int x = -1; x < 2; x++)
    {
        [unroll]
        for (int y = -1; y < 2; y++)
        {
            shadow += shadowTexture.SampleCmpLevelZero(shadowSampler, shadowPos.xy + float2(x, y) * texelSize, shadowPos.z);
        }
    }
    
    shadow /= 9.0;
    
    return 1.0 - shadow;
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float3 position = gPosition.Sample(linearClampSampler, texCoord).xyz;
    const float4 normalSpecular = gNormalSpecular.Sample(linearClampSampler, texCoord);
    const float3 baseColor = gBaseColor.Sample(linearClampSampler, texCoord).rgb;
    
    float alpha = gPosition.Sample(linearClampSampler, texCoord).a;
    
    if (alpha < 0.8)
    {
        return float4(0.0, 0.0, 0.0, 1.0);
    }
    
    float3 outColor = float3(0.0, 0.0, 0.0);
    
    const float3 N = normalize(normalSpecular.rgb);
    const float3 V = normalize(perframeResource.eyePos.xyz - position);
    
    const float NdotL = max(0.0, dot(N, volume.lightDir.xyz));
    const float3 diffuseColor = volume.lightColor.rgb * baseColor.rgb * NdotL;
        
    const float3 H = normalize(V + volume.lightDir.xyz);
    const float NdotH = max(dot(N, H), 0.0);
    const float3 specularColor = volume.lightColor.rgb * normalSpecular.w * pow(NdotH, 32.0);
   
    float shadow = CalShadow(position);
    
    outColor += (diffuseColor + specularColor) * shadow;
    
    if (shadow < 0.2)
    {
        outColor += baseColor * 0.10;
    }
    
    //outColor += baseColor * GetIndirectDiffuse(position, N, volume, irradianceCoeff, depthOctahedralMap, clampSampler);
    
    //const float ao = ssaoTexture.Sample(clampSampler, texCoord).r;
        
    //outColor *= ao;
    
    return float4(outColor, 1.0);
}