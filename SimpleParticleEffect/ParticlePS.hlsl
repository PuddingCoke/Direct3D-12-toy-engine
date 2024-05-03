struct PixelInput
{
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
    float intensity = 0.5 - length(float2(0.5, 0.5) - input.uv);
    
    clip(intensity);
    
    intensity = clamp(intensity, 0.0, 0.5) * 4.0;
    
    float3 color = lerp(input.color.rgb, input.color.rgb * 0.1, 1.0 - intensity);
    
    return float4(color, 1.0);
}