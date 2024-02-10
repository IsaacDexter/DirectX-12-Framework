#pragma once
#include "stdafx.h"
#include "Resource.h"
#include <string>

class Texture : public Resource
{
public:
	/**
	* It is not the texture's place to handle the SRV & CBV heap, so designate it free handles elsewhere beforehand.
	* @param srvCpuDescriptorHandle free CPU handle on the Descriptor heap to designate to the textures SRV.
	* @param srvGpuDescriptorHandle free GPU handle on the Descriptor heap to designate to the textures SRV.
	* @param srvRootParameterIndex the root parameter index to the SRV descriptor table in which the Texture lies.
	*/
	Texture(const ResourceHandle resourceHandle, const UINT& srvRootParameterIndex, const wchar_t* path);
	/**
	* Loads and creates a given texture into a resource.
	* @param uploadRes, ComPtr to an upload resource, that needs to stay in scope past the method. Ensure the GPU is flushed so it doesn't leave scope prematurely.
	* @param path, literal to the texture files location
	* @param descriptor UINT identifier to the texture
	* @ returns the newly created texture resource
	*/
	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	std::string GetPath();

protected:
	// Note: ComPtr's are CPU objects but this resource needs to stay in scope until
	// the command list that references it has finished executing on the GPU.
	// We will flush the GPU at the end of this method to ensure the resource is not
	// prematurely destroyed.
	Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadRes;
	const std::wstring m_path;
};

