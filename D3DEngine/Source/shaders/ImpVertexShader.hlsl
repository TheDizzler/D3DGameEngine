//cbuffer ApplicationBuffer : register(b0) {
//	matrix projectionMatrix;
//};

cbuffer PerFrameBuffer : register(b0) {
	matrix viewProjectionMatrix;
};

//cbuffer PerObjectBuffer : register(b2) {
//	matrix worldMatrix;
//};

/* Object that gets delivered from Input Assembler. */
struct AppData {
	// per-vertex data
	float3 position : POSITION;			// 12 Bytes + 4 Bytes padding
	float3 normal : NORMAL;				// 12 Bytes + 4 Bytes padding
	float2 texCoord : TEXCOORD;			// 8 Bytes + 8 Bytes padding
	float3 tangent : TANGENT;			// 12 Bytes + 4 Bytes padding

	// per-instance data
	matrix worldMatrix : WORLDMATRIX;	// 64 Bytes
	matrix inverseTranspose : INVERSETRANSPOSEWOLRDMATRIX;
};	// Total:							// 192 Bytes ?

/* Object that gets sent to Rasterizer from here. */
struct VertexShaderOutput {
	float4 positionWS   : TEXCOORD1;	// 16 Bytes
	float3 normalWS     : TEXCOORD2;	// 12 Bytes + 4 Bytes padding
	float2 texCoord     : TEXCOORD0;	// 8 Bytes	+ 8 Bytes padding
	float4 position     : SV_Position;	// 16 Bytes
};	// Total:							// 64 Bytes


VertexShaderOutput ImpVertexShader(AppData IN) {

	VertexShaderOutput OUT;

	matrix mvp = mul(viewProjectionMatrix, IN.matrix);

	OUT.position = mul(mvp, float4(IN.position, 1.0f));
	OUT.positionWS = mul(IN.matrix, float4(IN.position, 1.0f));
	OUT.NormalWS = mul((float3x3)IN.inverseTranspose, IN.Normal);
	OUT.texCoord = IN.texCoord;

	//// Calculate the normal vector against the world matrix only.
	//OUT.normal = mul(IN.normal, (float3x3)worldMatrix);
	//OUT.normal = normalize(OUT.normal);

	//// not sure if this done right
	//OUT.tangent = mul(IN.tangent, (float3x3) worldMatrix);
	//OUT.tangent = normalize(OUT.tangent);

	return OUT;
}