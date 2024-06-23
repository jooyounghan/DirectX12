struct BasicVertexInput
{
    float3 f3WorldPos : POSITION;
    float4 f4Color : COLOR;
};

struct BasicVertexOutput
{
    float4 f4WorldPos : SV_POSITION;
    float4 f4Color : COLOR;
};


BasicVertexOutput main(BasicVertexInput pos)
{
    BasicVertexOutput result;
    result.f4WorldPos = float4(pos.f3WorldPos, 1.f);
    result.f4Color = pos.f4Color;
    return result;
}