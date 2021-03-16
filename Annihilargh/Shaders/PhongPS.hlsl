// can change position of point light every frame
cbuffer LightConstBuffer
{
	float3 lightPos;
}

// for now these are not going to change during runtime, so they're static.
// later we can make these dynamic using a constant buffer
static const float3 materialColour = {.9f,.9f,.9f};
static const float3 diffuseColour = {1.f,1.f,1.f};
static const float diffuseIntensity = 1.f;
static const float3 ambient = {.0f,.0f,.0f};
static const float attenuationConstant = 1.f;
static const float attenuationLinear = 0.027f;
static const float attenuationQuadratic = 0.0028f;

// takes in the pixel's position relative to the world and the normal of the pixel
float4 main(float3 worldPos : Position, float3 norm : Normal) : SV_TARGET
{
	// acquire light vector information
	const float3 vertexToLight = lightPos - worldPos;
	const float distToLight = length(vertexToLight);
	const float3 normalizedVertexToLight = vertexToLight/distToLight;
	// diffuse attenuation
	const float attenuation = 1.f/(attenuationConstant + attenuationLinear * distToLight + attenuationQuadratic *
		(distToLight*distToLight));
	// diffuse intensity
	// using * between two vectors in hlsl gets the hadamard product (multiply each element)
	// So to get dot product you use dot()
	const float3 diffuse = diffuseColour * diffuseIntensity * attenuation * max(0.f,dot(normalizedVertexToLight,norm));
	// calculate final colour
	// saturate() takes in a scaler, vector, or matrix, and clamps it between 0 and 1
	return float4(saturate(diffuse+ambient)*materialColour,1.f);
}
