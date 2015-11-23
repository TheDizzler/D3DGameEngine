#include "Model3D.h"



Model::Model() {

	worldMatrix = XMMatrixIdentity();
}


Model::~Model() {

	QuickMessage(L"Release check");
	for (MeshData mesh : meshData) {
		mesh.release();
	}

}

void Model::render(ID3D11DeviceContext * deviceContext, ID3D11Buffer* constantBuffers[]) {

	// set constant buffers containing parameters specific to the mesh (ex: worldMatrix)
	deviceContext->UpdateSubresource(constantBuffers[PerObjectBuffer], 0, nullptr, &worldMatrix, 0, 0);
	deviceContext->VSSetConstantBuffers(2, 1, &constantBuffers[PerObjectBuffer]);

	
	for (MeshData data : meshData) {

		data.render(deviceContext);
	}
}


void Model::MeshData::render(ID3D11DeviceContext * deviceContext) {

	// set constant buffers to use textures, samplers, etc needed for material
	//deviceContext->VSSetConstantBuffers();
	//deviceContext->PSSetConstantBuffers();
	ID3D11ShaderResourceView* tex;
	if (hasTexture)
		tex = texture.getTexture();
	else
		tex = NULL; // things fucked up somewhere. Should set a default texture
	deviceContext->PSSetShaderResources(0, 1, &tex);

	


	// set context to use vertex & index buffers for mesh
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	// do we need an inputlayout for every object?
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // do we need this for every object?
	deviceContext->DrawIndexed(numIndices, 0, 0);

}

void Model::MeshData::release() {
		if (vertexBuffer)
			vertexBuffer->Release();
		if (indexBuffer)
			indexBuffer->Release();
		//QuickMessage(L"released");
}

