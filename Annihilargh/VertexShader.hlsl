struct VSOut
{
	float3 colour : Colour;	
	float4 pos : SV_Position;
};

cbuffer cb
{
	row_major matrix transform; // matrix represents 4x4 float
};

VSOut main(float2 pos : Position,float3 colour : Colour)
{
	VSOut vso;
	vso.pos = mul(float4(pos.x,pos.y,0.f,1.f),transform);
	vso.colour=colour;
	return vso;
}