struct PixelShaderInput {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
}

cbuffer DiffuseLightBuffer {
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};


float4 ImpPixelShader(PixelShaderInput IN) : SV_TARGET {

	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;

	color = ambientColor; // default color for all pixels

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, IN.tex);
	lightDir = -lightDirection;

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(IN.normal, lightDir));

	if (lightIntensity > 0.0f) {
		// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
		color += (diffuseColor * lightIntensity);
	}

	// saturate light as combiniation of ambient + diffuse could be > 1
	color = saturate(color);

	// Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	color = color * textureColor;


	return color;
}