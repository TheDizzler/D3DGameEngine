#define MAX_LIGHTS 8

// Light types
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

Texture2D shaderTexture : register(t0);
sampler samplerState : register(s0);

struct Material {
	float4  emissive;       // 16 bytes
							//----------------------------------- (16 byte boundary)
	float4  ambient;        // 16 bytes
							//------------------------------------(16 byte boundary)
	float4  diffuse;        // 16 bytes
							//----------------------------------- (16 byte boundary)
	float4  specular;       // 16 bytes
							//----------------------------------- (16 byte boundary)
	float   specularPower;  // 4 bytes
	bool    hasTexture;     // 4 bytes
	float2  padding;        // 8 bytes
							//----------------------------------- (16 byte boundary)
};  // Total:               // 80 bytes ( 5 * 16 )


cbuffer MaterialProperties : register(b0) {
	Material material;
};

struct Light {
	float4      position;               // 16 bytes
										//----------------------------------- (16 byte boundary)
	float4      direction;              // 16 bytes
										//----------------------------------- (16 byte boundary)
	float4      color;                  // 16 bytes
										//----------------------------------- (16 byte boundary)
	float       spotAngle;              // 4 bytes
	float       constantAttenuation;    // 4 bytes
	float       linearAttenuation;      // 4 bytes
	float       quadraticAttenuation;   // 4 bytes
										//----------------------------------- (16 byte boundary)
	int         lightType;              // 4 bytes
	bool        enabled;                // 4 bytes
	int2        padding;                // 8 bytes
										//----------------------------------- (16 byte boundary)
};  // Total:                           // 80 bytes (5 * 16 byte boundary)

cbuffer LightProperties : register(b1) {
	float4 eyePosition;                 // 16 bytes
										//----------------------------------- (16 byte boundary)
	float4 globalAmbient;               // 16 bytes
										//----------------------------------- (16 byte boundary)
	Light lights[MAX_LIGHTS];           // 80 * 8 = 640 bytes
};  // Total:                           // 672 bytes (42 * 16 byte boundary)


float4 doDiffuse(Light light, float3 L, float3 N) {
	float nDotL = max(0, dot(N, L));
	return light.color * nDotL;
}


float4 doSpecular(Light light, float3 V, float3 L, float3 N) {
	// Phong lighting
	float3 r = normalize(reflect(-L, N));
	float rDotV = max(0, dot(R, V));

	// Blinn-Phong lighting
	//float3 h = normalize(L + V);
	//float nDotH = max(0, dot(N, H));

	return light.color * pow(rDotV, Material.specularPower);
}


float doAttenuation(Light light, float d) {
	return 1.0f / (light.constantAttenuation + light.linearAttenuation * d + light.quadraticAttenuation * d * d);
}


struct LightingResult {
	float4 diffuse;
	float4 specular;
};

LightingResult doPointLight(Light light, float3 V, float4 P, float3 N) {

	LightingResult result;

	float3 L = (light.position - P).xyz;
	float distance = length(L);
	L = L / distance;

	float attenuation = doAttenuation(light, distance);

	result.diffuse = doDiffuse(light, L, N) * attenuation;
	result.specular = doSpecular(light, V, L, N) * attenuation;

	return result;
}

LightingResult doDirectionalLight(Light light, float3 V, float4 P, float3 N) {

	LightingResult result;

	float3 L = -light.direction.xyz;

	result.diffuse = doDiffuse(light, L, N);
	result.specular = doSpecular(light, V, L, N);

	return result;
}

float doSpotCone(Light light, float3 L) {

	float spotMinAngle = cos(light.spotAngle);
	float spotMaxAngle = (spotMinAngle + 1.0f) / 2.0f;
	float cosAngle = dot(light.direction.xyz, L);
	return smoothstep(spotMinAngle, spotMaxAngle, cosAngle);
}

LightingResult doSpotLight(Light light, float3 V, float4 P, float3 N) {

	LightingResult result;

	float3 L = (light.position - P).xyz;
	float distance = length(L);
	L = L / distance;

	float attenuation = doAttenuation(light, distance);
	float spotIntensity = doSpotCone(light, -L);

	result.diffuse = doDiffuse(light, L, N) * attenuation * spotIntensity;
	result.specular = doSpecular(light, V, L, N) * attenuation * spotIntensity;

	return result;
}

LightingResult computeLighting(float4 P, float3 N) {

	float3 V = normalize(EyePosition - P).xyz;

	LightingResult totalResult = {{0, 0, 0, 0},{0, 0, 0, 0}};

	[unroll]
	for (int i = 0; i < MAX_LIGHTS; ++i) {
		LightingResult result = {{0, 0, 0, 0},{0, 0, 0, 0}};

		if (!Lights[i].enabled) continue;

		switch (Lights[i].lightType) {
			case DIRECTIONAL_LIGHT:
				result = doDirectionalLight(Lights[i], V, P, N);
				break;
			case POINT_LIGHT:
				result = doPointLight(Lights[i], V, P, N);
				break;
			case SPOT_LIGHT:
				result = doSpotLight(Lights[i], V, P, N);
				break;
		}
		totalResult.diffuse += result.diffuse;
		totalResult.specular += result.specular;
	}

	totalResult.diffuse = saturate(totalResult.diffuse);
	totalResult.specular = saturate(totalResult.specular);

	return totalResult;
}


struct PixelShaderInput {
	float4 positionWS   : TEXCOORD1;
	float3 normalWS     : TEXCOORD2;
	float2 texCoord     : TEXCOORD0;
};



float4 TexturedLitPixelShader(PixelShaderInput IN) : SV_TARGET{

	LightingResult lit = computeLighting(IN.positionWS, normalize(IN.normalWS));

	float4 emissive = Material.emissive;
	float4 ambient = Material.ambient * globalAmbient;
	float4 diffuse = Material.diffuse * lit.diffuse;
	float4 specular = Material.specular * lit.specular;

	float4 texColor = {1, 1, 1, 1};

	if (Material.hasTexture) {
		texColor = shaderTexture.Sample(samplerState, IN.texCoord);
	}

	float4 finalColor = (emissive + ambient + diffuse + specular) * texColor;

	return finalColor;
}
