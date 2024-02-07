#pragma once
#include "stdafx.h"

class ConstantBuffer
{
public:
	ConstantBuffer(const D3D12_CPU_DESCRIPTOR_HANDLE& cbvCpuDescriptorHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& cbvGpuDescriptorHandle, const UINT& cbvRootParameterIndex);
	void Initialize(ID3D12Device* device);
	// Update Model View Projection (MVP) Matrix according to camera position
	void Update(DirectX::XMMATRIX model, DirectX::XMMATRIX view, DirectX::XMMATRIX projection);
	void Set(ID3D12GraphicsCommandList* commandList);
private:
	// Constant buffer used to translate the triangle in the shaders
	struct SceneConstantBuffer
	{
		// Constant buffer must be 256-Byte aligned 
		// Model View Projection matrix
		DirectX::XMFLOAT4X4 mvp;	//4 * 4 = 16 Bytes
		float padding[48];	//60 * 4 =	240 Bytes
	};
	// Ensure constant buffer is 256-byte aligned
	static_assert((sizeof(SceneConstantBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");

	const D3D12_CPU_DESCRIPTOR_HANDLE m_cpuDescriptorHandle;
	const D3D12_GPU_DESCRIPTOR_HANDLE m_gpuDescriptorHandle;
	const UINT m_rootParameterIndex;

	// Constant buffer
	Microsoft::WRL::ComPtr<ID3D12Resource> m_constantBuffer;
	SceneConstantBuffer m_constantBufferData;
	UINT8* m_pCbvDataBegin;
};

