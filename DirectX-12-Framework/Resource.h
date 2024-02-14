#pragma once
#include "stdafx.h"
#include <string>

struct Resource
{
	const D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle;
	const D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;
	const UINT rootParameterIndex;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	std::string name;

	Resource(const D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle, const D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle, const UINT rootParameterIndex)
		: cpuDescriptorHandle(cpuDescriptorHandle)
		, gpuDescriptorHandle(gpuDescriptorHandle)
		, rootParameterIndex(rootParameterIndex)
		, name()
	{}

	void Set(ID3D12GraphicsCommandList* commandList)
	{
		commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuDescriptorHandle);
	}
};

