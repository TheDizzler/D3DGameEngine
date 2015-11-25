cbuffer PerObject : register(b0) {
	matrix worldMatrix;
	matrix inverseTransposeWorldMatrix;
	matrix worldViewProjectionMatrix;
}

struct AppData {
	float3 position : POSITION;		// 12 Bytes
	float3 normal   : NORMAL;		// 12 Bytes
	float2 texCoord : TEXCOORD;		// 12 Bytes
}; // Total:						// 48 Bytes = (12 Bytes + 4 Bytes padding) * 3

struct VertexShaderOutput {
	float4 positionWS   : TEXCOORD1;	// 16 Bytes
	float3 normalWS     : TEXCOORD2;	// 12 Bytes
	float2 texCoord     : TEXCOORD0;	// 8 Bytes
	float4 position     : SV_Position;	// 16 Bytes
};	// Total:							// 64 Bytes


VertexShaderOutput SimpleVertexShader(AppData IN) {

	VertexShaderOutput OUT;

	OUT.position = mul(worldViewProjectionMatrix, float4(IN.position, 1.0f));
	OUT.positionWS = mul(worldMatrix, float4(IN.position, 1.0f));
	OUT.normalWS = mul((float3x3)inverseTransposeWorldMatrix, IN.normal);
	OUT.texCoord = IN.texCoord;

	return OUT;
}