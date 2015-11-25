cbuffer PerFrame : register(b0) {
	matrix viewProjectionMatrix;
}

struct AppData {
	// Per-vertex data
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float2 texCoord : TEXCOORD;
	// Per-instance data
	matrix matrix   : WORLDMATRIX;
	matrix inverseTranspose : INVERSETRANSPOSEWORLDMATRIX;
};






VertexShaderOutput InstancedVertexShader(AppData IN) {

	VertexShaderOutput OUT;

	matrix MVP = mul(viewProjectionMatrix, IN.matrix);

	OUT.position = mul(MVP, float4(IN.position, 1.0f));
	OUT.positionWS = mul(IN.matrix, float4(IN.position, 1.0f));
	OUT.normalWS = mul((float3x3)IN.inverseTranspose, IN.normal);
	OUT.texCoord = IN.texCoord;

	return OUT;
}