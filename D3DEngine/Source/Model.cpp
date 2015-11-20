#include "Model.h"



Model::Model() {

	worldMatrix = XMMatrixIdentity();
}


Model::~Model() {

	for (MeshData mesh : meshData) {
		mesh.release();
	}

}

void Model::render(ID3D11DeviceContext * deviceContext) {

	// set constant buffers containing parameters specific to the mesh (ex: worldMatrix)
	//deviceContext->VSSetConstantBuffers();

	for (MeshData data : meshData) {

		data.render(deviceContext);
	}
}


void Model::MeshData::render(ID3D11DeviceContext * deviceContext) {

	// set constant buffers to use textures, samplers, etc needed for material
	//deviceContext->VSSetConstantBuffers();
	//deviceContext->PSSetConstantBuffers();
	//deviceContext->PSSetShaderResources();
	//deviceContext->PSSetSamplers();


	// set context to use vertex & index buffers for mesh
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	// do we need an inputlayout here?
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // do we need this?
	deviceContext->DrawIndexed(numIndices, 0, 0);

}

