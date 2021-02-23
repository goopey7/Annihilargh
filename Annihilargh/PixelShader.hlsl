cbuffer PixelConstantBuffer
{
	float4 faceColours[6];
}

// system will assign an id for each triangle dividing by two ensures that two consecutive IDs get the same colour.
float4 main(uint triId : SV_PrimitiveID) : SV_TARGET
{
	return faceColours[triId/2];
}