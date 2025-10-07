//修改自https://www.shadertoy.com/view/lstSRS 作者：sonicether

#include"Common.hlsli"

#define ITERATIONS 256

cbuffer RenderParameters : register(b2)
{
    uint noiseTextureIndex;
    uint diskTextureIndex;
    float2 iResolution;
    float iTime;
    float period;
}

static Texture2D noiseTexture = ResourceDescriptorHeap[noiseTextureIndex];

static Texture2D diskTexture = ResourceDescriptorHeap[diskTextureIndex];

static const float3 MainColor = float3(1.0, 1.0, 1.0);

static const float2 iMouse = float2(0.0, 0.0);

float pcurve(float x, float a, float b)
{
    float k = pow(a + b, a + b) / (pow(a, a) * pow(b, b));
    return k * pow(x, a) * pow(1.0 - x, b);
}

float sdTorus(float3 p, float2 t)
{
    float2 q = float2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

void Haze(inout float3 color, float3 pos, float alpha)
{
    float2 t = float2(1.0, 0.01);

    float torusDist = length(sdTorus(pos + float3(0.0, -0.05, 0.0), t));

    float bloomDisc = 1.0 / (pow(torusDist, 2.0) + 0.001);
    float3 col = MainColor;
    bloomDisc *= length(pos) < 0.5 ? 0.0 : 1.0;

    color += col * bloomDisc * (2.9 / float(ITERATIONS)) * (1.0 - alpha * 1.0);
}

float3 hash(float3 p, float scale) // replace this by something better
{
    p = fmod(p, scale);
    
    p = float3(dot(p, float3(127.1, 311.7, 74.7)),
			  dot(p, float3(269.5, 183.3, 246.1)),
			  dot(p, float3(113.5, 271.9, 124.6)));

    return -1.0 + 2.0 * frac(sin(p) * 43758.5453123);
}

// return value noise (in x) and its derivatives (in yzw)
float4 noised(in float3 x, in float scale)
{
    x *= scale;

    // grid
    float3 i = floor(x);
    float3 w = frac(x);
    
#if 1
    // quintic interpolant
    float3 u = w * w * w * (w * (w * 6.0 - 15.0) + 10.0);
    float3 du = 30.0 * w * w * (w * (w - 2.0) + 1.0);
#else
    // cubic interpolant
    float3 u = w*w*(3.0-2.0*w);
    float3 du = 6.0*w*(1.0-w);
#endif    
    
    // gradients
    float3 ga = hash(i + float3(0.0, 0.0, 0.0), scale);
    float3 gb = hash(i + float3(1.0, 0.0, 0.0), scale);
    float3 gc = hash(i + float3(0.0, 1.0, 0.0), scale);
    float3 gd = hash(i + float3(1.0, 1.0, 0.0), scale);
    float3 ge = hash(i + float3(0.0, 0.0, 1.0), scale);
    float3 gf = hash(i + float3(1.0, 0.0, 1.0), scale);
    float3 gg = hash(i + float3(0.0, 1.0, 1.0), scale);
    float3 gh = hash(i + float3(1.0, 1.0, 1.0), scale);
    
    // projections
    float va = dot(ga, w - float3(0.0, 0.0, 0.0));
    float vb = dot(gb, w - float3(1.0, 0.0, 0.0));
    float vc = dot(gc, w - float3(0.0, 1.0, 0.0));
    float vd = dot(gd, w - float3(1.0, 1.0, 0.0));
    float ve = dot(ge, w - float3(0.0, 0.0, 1.0));
    float vf = dot(gf, w - float3(1.0, 0.0, 1.0));
    float vg = dot(gg, w - float3(0.0, 1.0, 1.0));
    float vh = dot(gh, w - float3(1.0, 1.0, 1.0));
	
    // interpolations
    return float4(va + u.x * (vb - va) + u.y * (vc - va) + u.z * (ve - va) + u.x * u.y * (va - vb - vc + vd) + u.y * u.z * (va - vc - ve + vg) + u.z * u.x * (va - vb - ve + vf) + (-va + vb + vc - vd + ve - vf - vg + vh) * u.x * u.y * u.z, // value
                 ga + u.x * (gb - ga) + u.y * (gc - ga) + u.z * (ge - ga) + u.x * u.y * (ga - gb - gc + gd) + u.y * u.z * (ga - gc - ge + gg) + u.z * u.x * (ga - gb - ge + gf) + (-ga + gb + gc - gd + ge - gf - gg + gh) * u.x * u.y * u.z + // derivatives
                 du * (float3(vb, vc, ve) - va + u.yzx * float3(va - vb - vc + vd, va - vc - ve + vg, va - vb - ve + vf) + u.zxy * float3(va - vb - ve + vf, va - vb - vc + vd, va - vc - ve + vg) + u.yzx * u.zxy * (-va + vb + vc - vd + ve - vf - vg + vh)));
}

//noise code by iq
float noise(in float3 x)
{
    float3 p = floor(x);
    float3 f = frac(x);
    f = f * f * (3.0 - 2.0 * f);
    float2 uv = (p.xy + float2(37.0, 17.0) * p.z) + f.xy;
    float2 rg = noiseTexture.Sample(linearWrapSampler, (uv + 0.5) / 256.0).yx;
    return -1.0 + 2.0 * lerp(rg.x, rg.y, f.z);
}

void GasDisc(inout float3 color, inout float alpha, float3 pos)
{
    float discRadius = 3.2;
    float discWidth = 5.3;
    float discInner = discRadius - discWidth * 0.5;
    float discOuter = discRadius + discWidth * 0.5;
    
    float3 origin = float3(0.0, 0.0, 0.0);
    float3 discNormal = normalize(float3(0.0, 1.0, 0.0));
    float discThickness = 0.1;

    float distFromCenter = distance(pos, origin);
    float distFromDisc = dot(discNormal, pos - origin);
    
    float radialGradient = 1.0 - saturate((distFromCenter - discInner) / discWidth * 0.5);

    float coverage = pcurve(radialGradient, 4.0, 0.9);

    discThickness *= radialGradient;
    coverage *= saturate(1.0 - abs(distFromDisc) / discThickness);

    float3 dustColorLit = MainColor;
    float3 dustColorDark = float3(0.0, 0.0, 0.0);

    float dustGlow = 1.0 / (pow(1.0 - radialGradient, 2.0) * 290.0 + 0.002);
    float3 dustColor = dustColorLit * dustGlow * 8.2;

    coverage = saturate(coverage * 0.7);


    float fade = pow((abs(distFromCenter - discInner) + 0.4), 4.0) * 0.04;
    float bloomFactor = 1.0 / (pow(distFromDisc, 2.0) * 40.0 + fade + 0.00002);
    float3 b = dustColorLit * pow(bloomFactor, 1.5);
    
    b *= lerp(float3(1.7, 1.1, 1.0), float3(0.5, 0.6, 1.0), pow(radialGradient, 2.0));
    b *= lerp(float3(1.7, 0.5, 0.1), float3(1.0, 1.0, 1.0), pow(radialGradient, 0.5));

    dustColor = lerp(dustColor, b * 150.0, saturate(1.0 - coverage * 1.0));
    coverage = saturate(coverage + bloomFactor * bloomFactor * 0.1);
    
    if (coverage < 0.01)
    {
        return;
    }
    
    float3 radialCoords;
    radialCoords.x = distFromCenter * 1.5 + 0.55;
    radialCoords.y = atan2(-pos.x, -pos.z) * 1.5;
    radialCoords.z = distFromDisc * 1.5;

    radialCoords *= 0.95;
    //speed = 1.0 / (period * 0.5 * 0.15)
    //float speed = 0.06;
    
    float speed = 1.0 / (period * 0.5 * 0.15);
    
    float noise1 = 1.0;
    float3 rc = radialCoords + 0.0;
    rc.y += iTime * speed;
    noise1 *= noised(rc * 3.0, 32.0) * 0.5 + 0.5;
    rc.y -= iTime * speed;
    noise1 *= noised(rc * 6.0, 32.0) * 0.5 + 0.5;
    rc.y += iTime * speed;
    noise1 *= noised(rc * 12.0, 32.0) * 0.5 + 0.5;
    rc.y -= iTime * speed;
    noise1 *= noised(rc * 24.0, 32.0) * 0.5 + 0.5;
    rc.y += iTime * speed;

    float noise2 = 2.0;
    rc = radialCoords + 30.0;
    noise2 *= noised(rc * 3.0, 32.0) * 0.5 + 0.5;
    rc.y += iTime * speed;
    noise2 *= noised(rc * 6.0, 32.0) * 0.5 + 0.5;
    rc.y -= iTime * speed;
    noise2 *= noised(rc * 12.0, 32.0) * 0.5 + 0.5;
    rc.y += iTime * speed;
    noise2 *= noised(rc * 24.0, 32.0) * 0.5 + 0.5;
    rc.y -= iTime * speed;
    noise2 *= noised(rc * 48.0, 32.0) * 0.5 + 0.5;
    rc.y += iTime * speed;
    noise2 *= noised(rc * 92.0, 32.0) * 0.5 + 0.5;
    rc.y -= iTime * speed;

    dustColor *= noise1 * 0.998 + 0.002;
    coverage *= noise2;
    
    radialCoords.y += iTime * speed * 0.5;
    
    //radialCoords.y = atan2(-pos.x, -pos.z) * 1.5;
    //radialCoords.y = (radialCoords.y + iTime * speed * 0.5) * 0.15
    //               = radialCoords.y * 0.15 + iTime * speed * 0.5 * 0.15
    //radialCoords.x = distFromCenter * 1.5 + 0.55
    //period = 1.0 / (speed * 0.5 * 0.15)
    //speed = 1.0 / (period * 0.5 * 0.15)
    
    dustColor *= pow(diskTexture.Sample(linearWrapSampler, radialCoords.yx * float2(0.15, 0.27)).rgb, float3(2.0, 2.0, 2.0)) * 4.0;
    
    coverage = saturate(coverage * 1200.0 / float(ITERATIONS));
    dustColor = max(float3(0.0, 0.0, 0.0), dustColor);

    coverage *= pcurve(radialGradient, 4.0, 0.9);

    color = (1.0 - alpha) * dustColor * coverage + color;

    alpha = (1.0 - alpha) * coverage + alpha;
}

float3 rotate(float3 p, float x, float y, float z)
{
    float3x3 matx = float3x3(1.0, 0.0, 0.0,
                     0.0, cos(x), sin(x),
                     0.0, -sin(x), cos(x));

    float3x3 maty = float3x3(cos(y), 0.0, -sin(y),
                     0.0, 1.0, 0.0,
                     sin(y), 0.0, cos(y));

    float3x3 matz = float3x3(cos(z), sin(z), 0.0,
                     -sin(z), cos(z), 0.0,
                     0.0, 0.0, 1.0);

    p = mul(matx, p);
    p = mul(matz, p);
    p = mul(maty, p);

    return p;
}

void RotateCamera(inout float3 eyevec, inout float3 eyepos)
{
    float mousePosY = iMouse.y / iResolution.y;
    float mousePosX = iMouse.x / iResolution.x;

    float3 angle = float3(mousePosY * 0.05 + 0.05, 1.0 + mousePosX * 1.0, -0.45);

    eyevec = rotate(eyevec, angle.x, angle.y, angle.z);
    eyepos = rotate(eyepos, angle.x, angle.y, angle.z);
}

void WarpSpace(inout float3 eyevec, inout float3 raypos)
{
    float3 origin = float3(0.0, 0.0, 0.0);

    float singularityDist = distance(raypos, origin);
    float warpFactor = 1.0 / (pow(singularityDist, 2.0) + 0.000001);

    float3 singularityVector = normalize(origin - raypos);
    
    float warpAmount = 5.0;

    eyevec = normalize(eyevec + singularityVector * warpFactor * warpAmount / float(ITERATIONS));
}

float rand(float2 coord)
{
    return saturate(frac(sin(dot(coord, float2(12.9898, 78.223))) * 43758.5453));
}

float4 main(float2 uv : TEXCOORD) : SV_TARGET
{
    float aspect = iResolution.x / iResolution.y;

    float2 uveye = uv;
    
    float3 eyevec = normalize(float3((uveye * 2.0 - 1.0) * float2(aspect, 1.0), 6.0));
    float3 eyepos = float3(0.0, -0.0, -10.0);
    
    float2 mousepos = iMouse.xy / iResolution.xy;
    if (mousepos.x == 0.0)
    {
        mousepos.x = 0.35;
    }
    eyepos.x += mousepos.x * 3.0 - 1.5;
    
    const float far = 15.0;

    RotateCamera(eyevec, eyepos);

    float3 color = float3(0.0, 0.0, 0.0);
    
    float dither = rand(uv) * 2.0;

    float alpha = 0.0;
    float3 raypos = eyepos + eyevec * dither * far / float(ITERATIONS);
    
    [loop]
    for (int i = 0; i < ITERATIONS; i++)
    {
        WarpSpace(eyevec, raypos);
        raypos += eyevec * far / float(ITERATIONS);
        GasDisc(color, alpha, raypos);
        Haze(color, raypos, alpha);
    }
    
    color *= 0.02;
    
    color = pow(color, float3(1.5, 1.5, 1.5));
    color = color / (1.0 + color);
    color = pow(color, float3(1.0 / 1.5, 1.0 / 1.5, 1.0 / 1.5));

    
    color = lerp(color, color * color * (3.0 - 2.0 * color), float3(1.0, 1.0, 1.0));
    color = pow(color, float3(1.3, 1.20, 1.0));

    color = saturate(color * 1.01);
    
    color = pow(color, float3(0.7 / 2.2, 0.7 / 2.2, 0.7 / 2.2));
    
    return float4(saturate(color), 1.0);
}