// can change position of point light every frame
// can now also change all aspects of light
cbuffer LightCB
{
	float3 lightPos;
	float3 diffuseColour;
	float diffuseIntensity;
	float3 ambient;
	float attenuationConstant;
	float attenuationLinear;
	float attenuationQuadratic;
}

// the colour of the object is a property of the object, not the light.
cbuffer DrawableCB
{
	float specularPower,specularIntensity;
	float padding[2];
}

Texture2D tex;
SamplerState smplr;

// takes in the pixel's position relative to the camera and the normal of the pixel relative to the camera
float4 main(float3 camPos : Position, float3 norm : Normal, float2 texCoord : TextureCoord) : SV_TARGET
{
	// acquire light vector information
	const float3 vertexToLight = lightPos - camPos;
	const float distToLight = length(vertexToLight);
	const float3 normalizedVertexToLight = vertexToLight/distToLight;
	// diffuse attenuation
	const float attenuation = 1.f/(attenuationConstant + attenuationLinear * distToLight + attenuationQuadratic *
		(distToLight*distToLight));
	// diffuse intensity
	// using * between two vectors in hlsl gets the hadamard product (multiply each element)
	// So to get dot product you use dot()
	const float3 diffuse = diffuseColour * diffuseIntensity * attenuation * max(0.f,dot(normalizedVertexToLight,norm));
	// calculate reflected light vector
	const float3 w = norm * dot(vertexToLight,norm);
	const float3 reflected = w * 2.f - vertexToLight;
	// calculate specular intensity using angle between camera view and reflected light
	const float3 specular = attenuation*diffuseColour*diffuseIntensity*specularIntensity*
		pow(max(.0f,dot(normalize(-reflected),normalize(camPos))),specularPower);
	// calculate final colour
	// saturate() takes in a scaler, vector, or matrix, and clamps it between 0 and 1
	return float4(saturate((diffuse+ambient)*tex.Sample(smplr,texCoord).rgb+specular),1.f);
}
