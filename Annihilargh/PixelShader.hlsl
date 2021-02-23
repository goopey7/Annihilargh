cbuffer PixelConstantBuffer
{
	float4 faceColours[6];
}

float4 main(uint triId : SV_PrimitiveID) : SV_TARGET
{
	return faceColours[triId/2];
}