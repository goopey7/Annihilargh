float4 main(float3 colour : Colour) : SV_TARGET
{
	return float4(colour,1.f);
}