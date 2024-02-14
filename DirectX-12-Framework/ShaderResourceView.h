#pragma once
#include "Resource.h"

struct ShaderResourceView : public Resource
{
	ShaderResourceView(const D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle, const D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle, const UINT rootParameterIndex)
		: Resource(cpuDescriptorHandle, gpuDescriptorHandle, rootParameterIndex)
	{}
	bool Load(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, const wchar_t* path);
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource;
};