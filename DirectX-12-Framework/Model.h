#pragma once
#include "stdafx.h"

class Model
{
public:
	Model();
	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	void CreateVertexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	void CreateIndexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView()
	{
		return m_vertexBufferView;
	}
	
	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView()
	{
		return m_indexBufferView;
	}
	
	const UINT& GetNumIndices()
	{
		return m_numIndices;
	}
private:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

	// Vertex & Index buffer
	Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_indexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
	UINT m_numIndices;
	// Create an upload heap to upload index data to the GPU index buffer heap
	Microsoft::WRL::ComPtr<ID3D12Resource> m_ibUploadHeap;
	// Create an upload heap to upload vertex data to the GPU vertex buffer heap
	Microsoft::WRL::ComPtr<ID3D12Resource> m_vbUploadHeap;
};

