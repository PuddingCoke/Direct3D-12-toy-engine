#include"Common.hlsli"

float4 main() : SV_TARGET
{
    return float4(cos(perframeResource.timeElapsed) * 0.5 + 0.5,
    sin(perframeResource.timeElapsed) * 0.5 + 0.5, 1.0f, 1.0f);
}
