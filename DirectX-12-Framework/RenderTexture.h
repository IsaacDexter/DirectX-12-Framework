#pragma once
#include "stdafx.h"
#include "Resource.h"

struct RenderTexture : public Resource
{
	/** 
	* Create a render texture, a combination Shader Resource View (SRV), Render Target View (RTV), and Depth Stencil View (DSV).
	* It is intended to be both drawn to (as an RTV) and drawn as an (SRV).
	* @param srvCpuDescriptorHandle CPU descriptor handle to where this' SRV is to be stored in the SRV heap
	* @param srvGpuDescriptorHandle GPU descriptor handle to where this' SRV is to be stored in the SRV heap
	* @param srvRootParameterIndex the root parameter index for all SRVs, RootParameterIndices::SRV
	* @param rtvCpuDescriptorHandle CPU descriptor handle to where this' RTV is to be stored in the RTV heap
	* @param dsvCpuDescriptorHandle CPU descriptor handle to where this' DSV is to be stored in the DSV heap
	*/
	RenderTexture(const D3D12_CPU_DESCRIPTOR_HANDLE srvCpuDescriptorHandle, const D3D12_GPU_DESCRIPTOR_HANDLE srvGpuDescriptorHandle, const UINT srvRootParameterIndex, const D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuDescriptorHandle, const D3D12_CPU_DESCRIPTOR_HANDLE dsvCpuDescriptorHandle)
		: Resource(srvCpuDescriptorHandle, srvGpuDescriptorHandle, srvRootParameterIndex)
		, rtvCpuDescriptorHandle(rtvCpuDescriptorHandle)
		, dsvCpuDescriptorHandle(dsvCpuDescriptorHandle)
	{}
	RenderTexture(Resource resource, const D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuDescriptorHandle, const D3D12_CPU_DESCRIPTOR_HANDLE dsvCpuDescriptorHandle)
		: Resource(resource)
		, rtvCpuDescriptorHandle(rtvCpuDescriptorHandle)
		, dsvCpuDescriptorHandle(dsvCpuDescriptorHandle)
	{}
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuDescriptorHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCpuDescriptorHandle;
	/** 
	* Creates this' SRV and RTV into the descriptor handles provided.
	* TODO : Also create DSV as opposed to piggybacking off an existing one.
	* @param device The ID3D12Device.
	*/
	void Initialize(ID3D12Device* device);

	/** 
	* Begin drawing to this render texture. Intended to sandwich general draw calls between RenderTexture::BeginDraw() and RenderTexture::EndDraw().
	* @param commandList A fresh, open command list, to be executed and reset independently after RenderTexture::EndDraw()
	*/
	void BeginDraw(ID3D12GraphicsCommandList* commandList);
	/** 
	* Cease drawing to this render texture. Intended to sandwich general draw calls between RenderTexture::BeginDraw() and RenderTexture::EndDraw().
	* @param commandList The same prepped command list from RenderTexture::BeginDraw(), to be executed and reset independently after this function.
	*/
	void EndDraw(ID3D12GraphicsCommandList* commandList);
};

