#pragma once
#include "stdafx.h"
#include "Resource.h"

struct ConstantBufferView : public Resource
{
public:
	ConstantBufferView(const D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle, const D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle, const UINT rootParameterIndex)
		: Resource(cpuDescriptorHandle, gpuDescriptorHandle, rootParameterIndex)
		, cbvDataBegin(nullptr)
		, cbvData()
	{}
	void Initialize(ID3D12Device* device);
	// Update Model View Projection (MVP) Matrix according to camera position
	void Update(const DirectX::XMMATRIX& model, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
protected:
	// Constant buffer used to translate the triangle in the shaders
	struct SceneConstantBuffer
	{
		// Constant buffer must be 256-Byte aligned 
		// Model View Projection matrix
		DirectX::XMFLOAT4X4 mvp;	//4 * 4 * 4 =	64 Bytes
		float padding[48];			//4 * 48 =		192 Bytes
	};
	// Ensure constant buffer is 256-byte aligned
	static_assert((sizeof(SceneConstantBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");

	SceneConstantBuffer cbvData;
	UINT8* cbvDataBegin;
};

