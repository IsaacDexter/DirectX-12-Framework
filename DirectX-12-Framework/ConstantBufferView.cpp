#include "ConstantBufferView.h"
using namespace DirectX;

void ConstantBufferView::Initialize(ID3D12Device* device)
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
			IID_PPV_ARGS(&resource)   // GUID of vertex buffer interface
		), "Failed to create constant buffer.\n");
		resource->SetName(L"constantBuffer");
	}

	{
		// Describe and create constant buffer view
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = resource->GetGPUVirtualAddress();  //GPU virtual address of constant buffer
		cbvDesc.SizeInBytes = constantBufferSize;   // Size of constant buffer
		// Create the constant buffer view, i.e. formatted constant buffer data and bind it to the CBV heap

		// Get a handle to the start of the shared SRV & CBV heap, which represents the free point in the heap. This will be incremented to add SRVs after CBVs to the heap.
		device->CreateConstantBufferView(&cbvDesc, cpuDescriptorHandle);


		// Map the constant buffer and initialize it
		// This can be mapped for the lifteime of the resource, isnt unmapped until app closes
		CD3DX12_RANGE readRange(0, 0);  // Range of reading resource on the CPU, which we needn't do
		ThrowIfFailed(resource->Map(0, &readRange, reinterpret_cast<void**>(&cbvDataBegin)), "Failed to map constant buffer.\n");
		memcpy(cbvDataBegin, &cbvData, sizeof(cbvData));   // map constant buffer data into the pointer to the beginning
	}

}

void ConstantBufferView::Update(const DirectX::XMMATRIX& model, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection)
{
	XMStoreFloat4x4(&cbvData.mvp, XMMatrixTranspose(model * view * projection));

	// Update the constant buffer pointer with new data
	memcpy(cbvDataBegin, &cbvData, sizeof(cbvData));
}