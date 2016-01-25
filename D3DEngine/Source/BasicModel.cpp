#include "BasicModel.h"

BasicModel::BasicModel(ID3D11Device * device) {

	effectFactory = unique_ptr<EffectFactory>(new EffectFactory(device));
	effectFactory->SetDirectory(Globals::ASSETS_DIR);

	//intializeBuffers(device);
}

BasicModel::~BasicModel() {

	
}

void BasicModel::changeDevice(ID3D11Device * device, ID3D11DeviceContext * deviceContext) {

	effectFactory.release();
	effectFactory = unique_ptr<EffectFactory>(new EffectFactory(device));
	effectFactory->SetDirectory(Globals::ASSETS_DIR);

	loadTextures(device, deviceContext, filename);
}

// Builds a look-at (world) matrix from a point, up and direction vectors.
XMMATRIX XM_CALLCONV LookAtMatrix(FXMVECTOR Position, FXMVECTOR Direction, FXMVECTOR Up) {

	XMVECTOR R2 = XMVector3Normalize(Direction);

	XMVECTOR R0 = XMVector3Cross(Up, R2);
	R0 = XMVector3Normalize(R0);

	XMVECTOR R1 = XMVector3Cross(R2, R0);

	XMMATRIX M(R0, R1, R2, Position);

	return M;
}


void BasicModel::update(float elapsedTime, Camera camera, ID3D11DeviceContext* deviceContext) {



	// Update the light properties
	XMStoreFloat4(&lightProperties.EyePosition, camera.getPosition());

	static float totalTime = 0.0f;


	static const XMVECTORF32 lightColors[MAX_LIGHTS] = {
		Colors::Red, Colors::Orange, Colors::Yellow, Colors::Green, Colors::Blue, Colors::Indigo, Colors::Violet, Colors::White
	};

	static const LightType lightTypes[MAX_LIGHTS] = {
		SpotLight, PointLight, SpotLight, PointLight, SpotLight, PointLight, SpotLight, PointLight
	};

	static const bool lightEnabled[MAX_LIGHTS] = {
		true, true, true, true, true, true, true, true
	};

	const int numLights = MAX_LIGHTS;
	float radius = 8.0f;
	float offset = 2.0f * XM_PI / numLights;

	for (int i = 0; i < numLights; ++i) {

		Light light;
		light.Enabled = static_cast<int>(lightEnabled[i]);
		light.LightType = lightTypes[i];
		light.Color = XMFLOAT4(lightColors[i]);
		light.SpotAngle = XMConvertToRadians(45.0f);
		light.ConstantAttenuation = 1.0f;
		light.LinearAttenuation = 0.08f;
		light.QuadraticAttenuation = 0.0f;
		XMFLOAT4 LightPosition = XMFLOAT4(std::sin(totalTime + offset * i) * radius, 9.0f,
			std::cos(totalTime + offset * i) * radius, 1.0f);
		light.Position = LightPosition;
		XMVECTOR LightDirection
			= XMVectorSet(-LightPosition.x, -LightPosition.y, -LightPosition.z, 0.0f);
		LightDirection = XMVector3Normalize(LightDirection);
		XMStoreFloat4(&light.Direction, LightDirection);

		lightProperties.Lights[i] = light;
	}

	// Update the light properties
	deviceContext->UpdateSubresource(lightPropertiesConstantBuffer.Get(), 0, NULL,
		&lightProperties, 0, 0);
}


void BasicModel::render(ID3D11DeviceContext * deviceContext, Camera camera,
	ID3D11RasterizerState* rasterizerState, ID3D11RenderTargetView* renderTargetView,
	ID3D11DepthStencilState* depthStencilState, ID3D11DepthStencilView* depthStencilView) {

	XMMATRIX viewMatrix = camera.getViewMatrix();
	XMMATRIX projectionMatrix = camera.getProjectionMatrix();
	XMMATRIX viewProjectionMatrix = viewMatrix * projectionMatrix;

	PerFrameConstantBufferData constantBufferData;
	constantBufferData.viewProjectionMatrix = viewProjectionMatrix;

	MaterialProperties wallMaterial = materialProperties[1];
	wallMaterial.Material.UseTexture = true;

	deviceContext->UpdateSubresource(perFrameConstantBuffer.Get(), 0, nullptr, &constantBufferData, 0, 0);
	deviceContext->UpdateSubresource(materialPropertiesConstantBuffer.Get(), 0, nullptr, &wallMaterial, 0, 0);

	const UINT vertexStride[2] = {sizeof(VertexPosNormTex), sizeof(InstanceData)};
	const UINT offset[2] = {0, 0};
	ID3D11Buffer* buffers[2] = {vertexBuffer.Get(), instanceBuffer.Get()};

	deviceContext->IASetVertexBuffers(0, 2, buffers, vertexStride, offset);
	deviceContext->IASetInputLayout(instancedInputLayout.Get());
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->RSSetState(rasterizerState);
	D3D11_VIEWPORT viewport = camera.getViewport();
	deviceContext->RSSetViewports(1, &viewport);

	deviceContext->VSSetShader(instancedVertexShader.Get(), nullptr, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &perFrameConstantBuffer);

	deviceContext->PSSetShader(texturedLitPixelShader.Get(), nullptr, 0);

	ID3D11Buffer* pixelShaderConstantBuffers[2]
		= {materialPropertiesConstantBuffer.Get(), lightPropertiesConstantBuffer.Get()};
	deviceContext->PSSetConstantBuffers(0, 2, pixelShaderConstantBuffers);

	deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	deviceContext->PSSetShaderResources(0, 1, &texture);

	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	deviceContext->OMSetDepthStencilState(depthStencilState, 0);

	deviceContext->DrawIndexedInstanced(_countof(planeIndex), numInstances, 0, 0, 0);

	// Draw the sphere
	XMMATRIX translationMatrix = XMMatrixTranslation(-4.0f, 2.0f, -4.0f);
	XMMATRIX rotationMatrix = XMMatrixIdentity();
	XMMATRIX scaleMatrix = XMMatrixScaling(4.0f, 4.0f, 4.0f);
	XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	PerObjectConstantBufferData perObjectConstantBufferData;
	perObjectConstantBufferData.WorldMatrix = worldMatrix;
	perObjectConstantBufferData.InverseTransposeWorldMatrix
		= XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix));
	perObjectConstantBufferData.WorldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;

	deviceContext->UpdateSubresource(perObjectConstantBuffer.Get(), 0, nullptr,
		&perObjectConstantBufferData, 0, 0);

	MaterialProperties sphereMaterial = materialProperties[0];
	sphereMaterial.Material.UseTexture = true;

	deviceContext->UpdateSubresource(materialPropertiesConstantBuffer.Get(),
		0, NULL, &sphereMaterial, 0, 0);

	deviceContext->VSSetShader(simpleVertexShader.Get(), nullptr, 0);
	deviceContext->VSSetConstantBuffers(0, 1, perObjectConstantBuffer.GetAddressOf());

	deviceContext->PSSetShaderResources(0, 1, tempTexture.GetAddressOf());

	deviceContext->IASetInputLayout(m_d3dVertexPositionNormalTextureInputLayout.Get());

	sphere->Draw(deviceContext);

	// Draw a cube
	translationMatrix = XMMatrixTranslation(4.0f, 4.0f, 4.0f);
	rotationMatrix = XMMatrixRotationY(XMConvertToRadians(45.0f));
	scaleMatrix = XMMatrixScaling(4.0f, 8.0f, 4.0f);
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	perObjectConstantBufferData.WorldMatrix = worldMatrix;
	perObjectConstantBufferData.InverseTransposeWorldMatrix
		= XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix));
	perObjectConstantBufferData.WorldViewProjectionMatrix
		= worldMatrix * viewProjectionMatrix;

	deviceContext->UpdateSubresource(perObjectConstantBuffer.Get(),
		0, nullptr, &perObjectConstantBufferData, 0, 0);

	deviceContext->UpdateSubresource(materialPropertiesConstantBuffer.Get(),
		0, nullptr, &materialProperties[2], 0, 0);

	cube->Draw(deviceContext);

	// Draw a torus
	translationMatrix = XMMatrixTranslation(4.0f, 0.5f, -4.0f);
	rotationMatrix = XMMatrixRotationY(XMConvertToRadians(45.0f));
	scaleMatrix = XMMatrixScaling(4.0f, 4.0f, 4.0f);
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	perObjectConstantBufferData.WorldMatrix = worldMatrix;
	perObjectConstantBufferData.InverseTransposeWorldMatrix
		= XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix));
	perObjectConstantBufferData.WorldViewProjectionMatrix
		= worldMatrix * viewProjectionMatrix;

	deviceContext->UpdateSubresource(perObjectConstantBuffer.Get(),
		0, nullptr, &perObjectConstantBufferData, 0, 0);
	deviceContext->UpdateSubresource(materialPropertiesConstantBuffer.Get(),
		0, nullptr, &materialProperties[3], 0, 0);

	torus->Draw(deviceContext);

	// Draw geometry at the position of the active lights in the scene.
	MaterialProperties lightMaterial = materialProperties[0];
	for (int i = 0; i < MAX_LIGHTS; ++i) {
		Light* pLight = &(lightProperties.Lights[i]);
		if (!pLight->Enabled) continue;

		XMVECTOR lightPos = XMLoadFloat4(&(pLight->Position));
		XMVECTOR lightDir = XMLoadFloat4(&(pLight->Direction));
		XMVECTOR UpDirection = XMVectorSet(0, 1, 0, 0);

		scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		rotationMatrix = XMMatrixRotationX(-90.0f);
		worldMatrix
			= scaleMatrix * rotationMatrix * LookAtMatrix(lightPos, lightDir, UpDirection);

		perObjectConstantBufferData.WorldMatrix = worldMatrix;
		perObjectConstantBufferData.InverseTransposeWorldMatrix
			= XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix));
		perObjectConstantBufferData.WorldViewProjectionMatrix
			= worldMatrix * viewProjectionMatrix;

		lightMaterial.Material.Emissive = pLight->Color;

		deviceContext->UpdateSubresource(perObjectConstantBuffer.Get(), 0, nullptr, &perObjectConstantBufferData, 0, 0);
		deviceContext->UpdateSubresource(materialPropertiesConstantBuffer.Get(), 0, nullptr, &lightMaterial, 0, 0);
		switch (pLight->LightType) {
			case PointLight:
				sphere->Draw(deviceContext);
				break;
			case DirectionalLight:
			case SpotLight:
				cone->Draw(deviceContext);
				break;
		}
	}

}

bool BasicModel::loadTextures(ID3D11Device * device, ID3D11DeviceContext * deviceContext, const WCHAR * file) {

	filename = file;

	try {
		effectFactory->CreateTexture(filename, deviceContext,
			texture.GetAddressOf());
	} catch (exception&) {
		stringstream str;
		str << "Failure to create texture from " << filename;
		ErrorMessage(str.str());
	}
	try {
		effectFactory->CreateTexture(L"earth.dds", deviceContext,
			tempTexture.GetAddressOf());
	} catch (exception&) {
		stringstream str;
		str << "Failure to create Earth texture.";
		ErrorMessage(str.str());
	}


	if (reportError(initSamplerState(device))) {

		ErrorMessage("Fdailed to init Sampler State");
		return false;
	}

	// Create some materials
	MaterialProperties defaultMaterial;
	materialProperties.push_back(defaultMaterial);

	MaterialProperties greenMaterial;
	greenMaterial.Material.Ambient = XMFLOAT4(0.07568f, 0.61424f, 0.07568f, 1.0f);
	greenMaterial.Material.Diffuse = XMFLOAT4(0.07568f, 0.61424f, 0.07568f, 1.0f);
	greenMaterial.Material.Specular = XMFLOAT4(0.07568f, 0.61424f, 0.07568f, 1.0f);
	greenMaterial.Material.SpecularPower = 76.8f;
	materialProperties.push_back(greenMaterial);

	MaterialProperties redPlasticMaterial;
	redPlasticMaterial.Material.Diffuse = XMFLOAT4(0.6f, 0.1f, 0.1f, 1.0f);
	redPlasticMaterial.Material.Specular = XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f);
	redPlasticMaterial.Material.SpecularPower = 32.0f;
	materialProperties.push_back(redPlasticMaterial);

	MaterialProperties pearlMaterial;
	pearlMaterial.Material.Ambient = XMFLOAT4(0.25f, 0.20725f, 0.20725f, 1.0f);
	pearlMaterial.Material.Diffuse = XMFLOAT4(1.0f, 0.829f, 0.829f, 1.0f);
	pearlMaterial.Material.Specular = XMFLOAT4(0.296648f, 0.296648f, 0.296648f, 1.0f);
	pearlMaterial.Material.SpecularPower = 11.264f;
	materialProperties.push_back(pearlMaterial);


	// Create and initialize a vertex buffer for a plane.
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(planeVerts);
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	resourceData.pSysMem = planeVerts;

	if (reportError(device->CreateBuffer(&vertexBufferDesc, &resourceData,
		vertexBuffer.GetAddressOf()))) {
		ErrorMessage("Failed to create vertex buffer.");
		return false;
	}


	// Create and setup the per-instance buffer data
	InstanceData* planeInstanceData
		= (InstanceData*) _aligned_malloc(sizeof(InstanceData) * numInstances, 16);

	float scalePlane = 20.0f;
	float translateOffset = scalePlane / 2.0f;
	XMMATRIX scaleMatrix = XMMatrixScaling(scalePlane, 1.0f, scalePlane);
	XMMATRIX translateMatrix = XMMatrixTranslation(0, 0, 0);
	XMMATRIX rotateMatrix = XMMatrixRotationX(0.0f);

	// Floor plane.
	XMMATRIX worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
	planeInstanceData[0].worldMatrix = worldMatrix;
	planeInstanceData[0].inverseTransposeWorldMatrix
		= XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix));

	// Back wall plane.
	translateMatrix = XMMatrixTranslation(0, translateOffset, translateOffset);
	rotateMatrix = XMMatrixRotationX(XMConvertToRadians(-90));
	worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;

	planeInstanceData[1].worldMatrix = worldMatrix;
	planeInstanceData[1].inverseTransposeWorldMatrix
		= XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix));

	// Ceiling plane.
	translateMatrix = XMMatrixTranslation(0, translateOffset * 2.0f, 0);
	rotateMatrix = XMMatrixRotationX(XMConvertToRadians(180));
	worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;

	planeInstanceData[2].worldMatrix = worldMatrix;
	planeInstanceData[2].inverseTransposeWorldMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix));

	// Front wall plane.
	translateMatrix = XMMatrixTranslation(0, translateOffset, -translateOffset);
	rotateMatrix = XMMatrixRotationX(XMConvertToRadians(90));
	worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;

	planeInstanceData[3].worldMatrix = worldMatrix;
	planeInstanceData[3].inverseTransposeWorldMatrix
		= XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix));

	// Left wall plane.
	translateMatrix = XMMatrixTranslation(-translateOffset, translateOffset, 0);
	rotateMatrix = XMMatrixRotationZ(XMConvertToRadians(-90));
	worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;

	planeInstanceData[4].worldMatrix = worldMatrix;
	planeInstanceData[4].inverseTransposeWorldMatrix
		= XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix));

	// Right wall plane.
	translateMatrix = XMMatrixTranslation(translateOffset, translateOffset, 0);
	rotateMatrix = XMMatrixRotationZ(XMConvertToRadians(90));
	worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;

	planeInstanceData[5].worldMatrix = worldMatrix;
	planeInstanceData[5].inverseTransposeWorldMatrix
		= XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix));


	// Create the per-instance vertex buffer.
	D3D11_BUFFER_DESC instanceBufferDesc;
	ZeroMemory(&instanceBufferDesc, sizeof(D3D11_BUFFER_DESC));

	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.ByteWidth = sizeof(InstanceData) * numInstances;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	resourceData.pSysMem = planeInstanceData;



	if (reportError(device->CreateBuffer(&instanceBufferDesc, &resourceData,
		instanceBuffer.GetAddressOf()))) {

		ErrorMessage("Failed to create instance buffer.");
		return false;
	}

	_aligned_free(planeInstanceData);


	// Create and setup the index buffer for a plane.
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(planeIndex);
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	resourceData.pSysMem = planeIndex;


	if (reportError(device->CreateBuffer(&indexBufferDesc, &resourceData, indexBuffer.GetAddressOf()))) {
		ErrorMessage("Failed to create index buffer.");
		return false;
	}

	ID3D10Blob* errorBlob;
	ID3D10Blob* vertexShaderBlob;
	ID3D10Blob* pixelShaderBlob;


	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	// Compile the instance vertex shader
	if (reportError(D3DCompileFromFile(INSTANCED_VERTEX_SHADER, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"InstancedVertexShader", Globals::VERTEX_SHADER_VERSION,
		flags, 0, &vertexShaderBlob, &errorBlob))) {
		if (errorBlob) {
			outputShaderErrorMessage(errorBlob, NULL, INSTANCED_VERTEX_SHADER);
		} else {
			MessageBox(NULL, INSTANCED_VERTEX_SHADER, L"Missing Vertex Shader File", MB_OK);
		}

		return false;
	}

	if (reportError(device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(), NULL, instancedVertexShader.GetAddressOf()))) {
		ErrorMessage("Failed to load instanced vertex shader.");
		return false;
	}




	// Create the input layout for rendering instanced vertex data.
	D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] = {
		// Per-vertex data.
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		// Per-instance data.
		{"WORLDMATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLDMATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLDMATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLDMATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INVERSETRANSPOSEWORLDMATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INVERSETRANSPOSEWORLDMATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INVERSETRANSPOSEWORLDMATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INVERSETRANSPOSEWORLDMATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	if (reportError(device->CreateInputLayout(vertexLayoutDesc, _countof(vertexLayoutDesc),
		vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(),
		&instancedInputLayout))) {
		ErrorMessage("Failed to create input layout.");
		return false;
	}



	if (reportError(D3DCompileFromFile(TEXTURED_LIT_PIXEL_SHADER, NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "TexturedLitPixelShader",
		Globals::PIXEL_SHADER_VERSION, flags, 0, &pixelShaderBlob, &errorBlob))) {
		if (errorBlob) {
			outputShaderErrorMessage(errorBlob, NULL, TEXTURED_LIT_PIXEL_SHADER);
		} else {
			MessageBox(NULL, TEXTURED_LIT_PIXEL_SHADER, L"Missing Pixel Shader File", MB_OK);
		}

		return false;
	}

	if (reportError(device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize(), NULL, texturedLitPixelShader.GetAddressOf()))) {
		ErrorMessage("Failed to load pixel shader.");
		return false;
	}


	// Create a constant buffer for the per-frame data required by the instanced vertex shader.
	D3D11_BUFFER_DESC constantBufferDesc;
	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(PerFrameConstantBufferData);
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	if (reportError(device->CreateBuffer(&constantBufferDesc, nullptr,
		perFrameConstantBuffer.GetAddressOf()))) {
		ErrorMessage("Failed to create constant buffer for per-frame data.");
		return false;
	}

	// Create a constant buffer for the per-object data required by the simple vertex shader.
	constantBufferDesc.ByteWidth = sizeof(PerObjectConstantBufferData);


	if (reportError(device->CreateBuffer(&constantBufferDesc, nullptr, &perObjectConstantBuffer))) {
		ErrorMessage("Failed to create constant buffer for per-object data.");
		return false;
	}

	// Create a constant buffer for the material properties required by the pixel shader.
	constantBufferDesc.ByteWidth = sizeof(MaterialProperties);


	if (reportError(device->CreateBuffer(&constantBufferDesc, nullptr,
		materialPropertiesConstantBuffer.GetAddressOf()))) {
		ErrorMessage("Failed to create constant buffer for material properties.");
		return false;
	}


	// Create a constant buffer for the light properties required by the pixel shader.
	constantBufferDesc.ByteWidth = sizeof(LightProperties);

	if (reportError(device->CreateBuffer(&constantBufferDesc, nullptr,
		lightPropertiesConstantBuffer.GetAddressOf()))) {
		ErrorMessage("Failed to create constant buffer for light properties.");
		return false;
	}


	// Global ambient
	lightProperties.GlobalAmbient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	sphere = PrimitiveGeometry::CreateSphere(deviceContext, 1.0f, 16, false);
	cube = PrimitiveGeometry::CreateCube(deviceContext, 1.0f, false);
	cone = PrimitiveGeometry::CreateCone(deviceContext, 1.0f, 1.0f, 32, false);
	torus = PrimitiveGeometry::CreateTorus(deviceContext, 1.0f, 0.33f, 32, false);

	ID3D10Blob* simpleShaderBlob;

	// Load a simple vertex shader that will be used to render the shapes.
	if (reportError(D3DCompileFromFile(SIMPLE_VERTEX_SHADER, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"SimpleVertexShader", Globals::VERTEX_SHADER_VERSION,
		flags, 0, &simpleShaderBlob, &errorBlob))) {
		if (errorBlob) {
			outputShaderErrorMessage(errorBlob, NULL, SIMPLE_VERTEX_SHADER);
		} else {
			MessageBox(NULL, SIMPLE_VERTEX_SHADER, L"Missing Vertex Shader File", MB_OK);
		}

		return false;
	}

	if (reportError(device->CreateVertexShader(simpleShaderBlob->GetBufferPointer(),
		simpleShaderBlob->GetBufferSize(), NULL, simpleVertexShader.GetAddressOf()))) {
		ErrorMessage("Failed to create the simple vertex shader.");
		return false;
	}

	// Create the input layout for the simple shapes.
	if (reportError(device->CreateInputLayout(VertexPositionNormalTexture::InputElements,
		VertexPositionNormalTexture::InputElementCount, simpleShaderBlob->GetBufferPointer(),
		simpleShaderBlob->GetBufferSize(), &m_d3dVertexPositionNormalTextureInputLayout))) {
		ErrorMessage("Failed to create the input layout for the simple vertex shader.");
		return false;
	}

	simpleShaderBlob->Release();
	vertexShaderBlob->Release();
	pixelShaderBlob->Release();

	return true;
}


bool BasicModel::initializeBuffers(ID3D11Device* device) {

	// Create and initialize a vertex buffer for a plane.
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(planeVerts);
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	resourceData.pSysMem = planeVerts;

	if (reportError(device->CreateBuffer(&vertexBufferDesc, &resourceData, vertexBuffer.GetAddressOf()))) {
		ErrorMessage("Failed to create vertex buffer.");
		return false;
	}

	return true;
}


HRESULT BasicModel::initSamplerState(ID3D11Device* device) {

	// Create a sampler state for texture sampling in the pixel shader
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	return device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());

}



void BasicModel::outputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd,
	const WCHAR* shaderFilename) {

	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	compileErrors = (char*) (errorMessage->GetBufferPointer());

	bufferSize = errorMessage->GetBufferSize();
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++) {
		fout << compileErrors[i];
	}
	OutputDebugStringA(compileErrors);
	fout.close();
	errorMessage->Release();
	MessageBox(NULL, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}