#pragma once
#include "stdafx.h"

struct ResourceHandle
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;
	ResourceHandle(const D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle, const D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle) :
		cpuDescriptorHandle(cpuDescriptorHandle),
		gpuDescriptorHandle(gpuDescriptorHandle)
	{

	}
};

class Resource
{
public:
	/**
	* It is not the texture's place to handle the SRV & CBV heap, so designate it free handles elsewhere beforehand.
	* @param srvCpuDescriptorHandle free CPU handle on the Descriptor heap to designate to the textures SRV.
	* @param srvGpuDescriptorHandle free GPU handle on the Descriptor heap to designate to the textures SRV.
	* @param srvRootParameterIndex the root parameter index to the SRV descriptor table in which the Texture lies.
	*/
	Resource(const ResourceHandle, const UINT& rootParameterIndex, std::string name);
	void Set(ID3D12GraphicsCommandList* commandList);

	const ResourceHandle& GetResourceHandle()
	{
		return m_resourceHandle;
	}
	ID3D12Resource* GetResource()
	{
		return m_resource.Get();
	}
	void SetResource(ID3D12Resource* resource)
	{
		m_resource = resource;
	}
	const UINT& GetRootParameterIndex()
	{
		return m_rootParameterIndex;
	}
	std::string GetName()
	{
		return m_name;
	}
protected:

	const ResourceHandle m_resourceHandle;
	const UINT m_rootParameterIndex;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
	std::string m_name;
};

