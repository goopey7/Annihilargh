cbuffer cb
{
	matrix transform; // matrix represents 4x4 float
};

float4 main(float3 pos : Position) : SV_Position
{
	return mul(float4(pos,1.f),transform);
}
