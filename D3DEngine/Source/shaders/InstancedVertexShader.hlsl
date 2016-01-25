cbuffer PerFrame : register(b0) {
	matrix viewProjectionMatrix;
}

struct AppData {
	// Per-vertex data
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float2 texCoord : TEXCOORD;
	// Per-instance data
	matrix worldMatrix   : WORLDMATRIX;
	matrix inverseTranspose : INVERSETRANSPOSEWORLDMATRIX;
};


struct VertexShaderOutput {
	float4 positionWS   : TEXCOORD1;
	float3 normalWS     : TEXCOORD2;
	float2 texCoord     : TEXCOORD0;
	float4 position     : SV_POSITION;
};



VertexShaderOutput InstancedVertexShader(AppData IN) {

	VertexShaderOutput OUT;

	matrix MVP = mul(viewProjectionMatrix, IN.worldMatrix);

	OUT.position = mul(MVP, float4(IN.position, 1.0f));
	OUT.positionWS = mul(IN.worldMatrix, float4(IN.position, 1.0f));
	OUT.normalWS = mul((float3x3) IN.inverseTranspose, IN.normal);
	OUT.texCoord = IN.texCoord;

	return OUT;
}