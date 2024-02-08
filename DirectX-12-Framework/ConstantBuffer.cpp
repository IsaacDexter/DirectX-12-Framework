#include "ConstantBuffer.h"
using namespace DirectX;

ConstantBuffer::ConstantBuffer(const D3D12_CPU_DESCRIPTOR_HANDLE& cbvCpuDescriptorHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& cbvGpuDescriptorHandle, const UINT& cbvRootParameterIndex) :
	Resource(cbvCpuDescriptorHandle, cbvGpuDescriptorHandle, cbvRootParameterIndex),
	m_pCbvDataBegin(nullptr),
	m_constantBufferData()
{
}

void ConstantBuffer::Initialize(ID3D12Device* device)
{

	const UINT constantBufferSize = sizeof(SceneConstantBuffer);
	// get the size of the constant buffer, which is already asserted to be 256-byte aligned
	{
		// Create upload heap to transfer constant buffer data to GPU
		auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);

		// Create the constant buffer in its own implicit heap
		ThrowIfFailed(device->CreateCommittedResource(
			&heapProps,  // The properties of the implicit heap
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,    // Descriptor for the resource
			D3D12_RESOURCE_STATE_GENERIC_READ,  // Required heap state for an upload heap
			nullptr,    // Optimized clear value for render target resources
			IID_PPV_ARGS(&m_resource)   // GUID of vertex buffer interface
		), "Failed to create constant buffer.\n");
		m_resource->SetName(L"m_constantBuffer");
	}

	{
		// Describe and create constant buffer view
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_resource->GetGPUVirtualAddress();  //GPU virtual address of constant buffer
		cbvDesc.SizeInBytes = constantBufferSize;   // Size of constant buffer
		// Create the constant buffer view, i.e. formatted constant buffer data and bind it to the CBV heap

		// Get a handle to the start of the shared SRV & CBV heap, which represents the free point in the heap. This will be incremented to add SRVs after CBVs to the heap.
		device->CreateConstantBufferView(&cbvDesc, m_cpuDescriptorHandle);


		// Map the constant buffer and initialize it
		// This can be mapped for the lifteime of the resource, isnt unmapped until app closes
		CD3DX12_RANGE readRange(0, 0);  // Range of reading resource on the CPU, which we needn't do
		ThrowIfFailed(m_resource->Map(0, &readRange, reinterpret_cast<void**>(&m_pCbvDataBegin)), "Failed to map constant buffer.\n");
		memcpy(m_pCbvDataBegin, &m_constantBufferData, sizeof(m_constantBufferData));   // map constant buffer data into the pointer to the beginning
	}

}

void ConstantBuffer::Update(const DirectX::XMMATRIX& model, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection)
{
	XMStoreFloat4x4(&m_constantBufferData.mvp, XMMatrixTranspose(model * view * projection));

	// Update the constant buffer pointer with new data
	memcpy(m_pCbvDataBegin, &m_constantBufferData, sizeof(m_constantBufferData));
}