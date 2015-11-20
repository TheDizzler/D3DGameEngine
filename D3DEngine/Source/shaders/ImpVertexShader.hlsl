cbuffer ApplicationBuffer : register(b0) {
	matrix projectionMatrix;
}

cbuffer PerFrameBuffer : register(b1) {
	matrix viewMatrix;
}

cbuffer PerObjectBuffer : register(b2) {
	matrix worldMatrix;
}

/* Object that gets delivered from Input Assembler. */
struct AppData {
	float3 position : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
}

/* Object that gets sent to Rasterizer from here. */
struct VertexShaderOutput {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
}


VertexShaderOutput ImpVertexShader(AppData IN) {

	VertexShaderOutput OUT;

	matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));
	OUT.position = mul(mvp, float4(IN.position, 1.0f));
	
	OUT.tex = input.tex;

	// Calculate the normal vector against the world matrix only.
	OUT.normal = mul(IN.normal, (float3x3)worldMatrix);
	OUT.normal = normalize(OUT.normal);

	// not sure if this done right
	OUT.tangent = mul(IN.tangent, (float3x3) worldMatrix);
	OUT.tangent = normalize(OUT.tangent);

	return OUT;
}