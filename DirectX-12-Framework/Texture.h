#pragma once
#include "stdafx.h"

class Texture
{
public:
	/**
	* It is not the texture's place to handle the SRV & CBV heap, so designate it free handles elsewhere beforehand.
	* @param cpuDescriptorHandle free CPU handle on the Descriptor heap to designate to the textures SRV.
	* @param gpuDescriptorHandle free GPU handle on the Descriptor heap to designate to the textures SRV.
	* @param the root parameter index to the SRV descriptor table in which the Texture lies.
	*/
	Texture(const D3D12_CPU_DESCRIPTOR_HANDLE& cpuDescriptorHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& gpuDescriptorHandle, const UINT& srvRootParameterIndex);
	/**
	* Loads and creates a given texture into a resource.
	* @param uploadRes, ComPtr to an upload resource, that needs to stay in scope past the method. Ensure the GPU is flushed so it doesn't leave scope prematurely.
	* @param path, literal to the texture files location
	* @param descriptor UINT identifier to the texture
	* @ returns the newly created texture resource
	*/
	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, const wchar_t* path);

	/**
	* Set this texture to be the current drawing texture in X command list 
	*/
	void Set(ID3D12GraphicsCommandList* commandList);

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuDescriptorHandle() const
	{
		return m_cpuDescriptorHandle;
	}
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGpuDescriptorHandle() const
	{
		return m_gpuDescriptorHandle;
	}
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_texture;
	const D3D12_CPU_DESCRIPTOR_HANDLE m_cpuDescriptorHandle;
	const D3D12_GPU_DESCRIPTOR_HANDLE m_gpuDescriptorHandle;
	const UINT m_rootParameterIndex;
	// Note: ComPtr's are CPU objects but this resource needs to stay in scope until
	// the command list that references it has finished executing on the GPU.
	// We will flush the GPU at the end of this method to ensure the resource is not
	// prematurely destroyed.
	Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadRes;
};

