struct BasicVertexOutput
{
    float4 f4WorldPos : SV_POSITION;
    float4 f4Color : COLOR;
};

float4 main(BasicVertexOutput pos) : SV_TARGET
{
    return pos.f4Color;

}