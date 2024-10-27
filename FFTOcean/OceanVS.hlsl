struct VertexIO
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

VertexIO main(VertexIO input)
{
    return input;
}