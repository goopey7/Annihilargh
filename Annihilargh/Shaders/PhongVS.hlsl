cbuffer ConstBuffer
{
	// model transform & model * view * projection
	matrix modelView;
	matrix modelViewProj;
}

struct VSOut
{
	float3 camPos : Position; 
	float3 normal : Normal;
	float2 texCoord : TextureCoord;
	float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 norm : Normal, float2 texCoord : TextureCoord)
{
	VSOut vso;
	vso.pos = mul(float4(pos,1.f),modelViewProj);
	// cast to 3x3 here because we only want to rotate the normals, not translate them
	vso.normal = mul(norm,(float3x3)modelView);
	vso.camPos = (float3)mul(float4(pos,1.f),modelView);
	vso.texCoord = texCoord;
	return vso;
}
