#pragma once
#include "stdafx.h"
class Resource
{
public:
	/**
	* It is not the texture's place to handle the SRV & CBV heap, so designate it free handles elsewhere beforehand.
	* @param srvCpuDescriptorHandle free CPU handle on the Descriptor heap to designate to the textures SRV.
	* @param srvGpuDescriptorHandle free GPU handle on the Descriptor heap to designate to the textures SRV.
	* @param srvRootParameterIndex the root parameter index to the SRV descriptor table in which the Texture lies.
	*/
	Resource(const D3D12_CPU_DESCRIPTOR_HANDLE& cpuDescriptorHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& gpuDescriptorHandle, const UINT& rootParameterIndex);
	void Set(ID3D12GraphicsCommandList* commandList);
protected:
	const D3D12_CPU_DESCRIPTOR_HANDLE m_cpuDescriptorHandle;
	const D3D12_GPU_DESCRIPTOR_HANDLE m_gpuDescriptorHandle;
	const UINT m_rootParameterIndex;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
};

