cbuffer ConstBuffer
{
	// model transform & model * view * projection
	matrix model,modelviewProj;
}

struct VSOut
{
	float4 pos : SV_Position;
	float3 normal : Normal;
	float3 worldPos : Position; 
};

VSOut main(float3 pos : Position, float3 norm : Normal)
{
	VSOut vso;
	vso.pos = mul(float4(pos,1.f),modelviewProj);
	// cast to 3x3 here because we only want to rotate the normals, not translate them
	vso.normal = mul(norm,(float3x3)model);
	vso.worldPos = (float3)mul(float4(pos,1.f),model);
	return vso;
}
