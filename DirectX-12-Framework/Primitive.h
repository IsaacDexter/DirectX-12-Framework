#pragma once
#include "stdafx.h"
#include <vector>

class Primitive
{
public:
	Primitive(std::string name);
	bool Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12PipelineState* pipelineState, ID3D12RootSignature* rootSignature, const wchar_t* path);
	void Draw(ID3D12GraphicsCommandList* commandList);

	std::string GetName()
	{
		return m_name;
	}
private:
	bool LoadModel(const wchar_t* path);
	void CreateVertexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	void CreateIndexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	void CreateBundle(ID3D12Device* device, ID3D12PipelineState* pipelineState, ID3D12RootSignature* rootSignature);
	
	
	
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

	typedef DWORD Index;
	std::vector<Vertex> m_vertices;
	UINT m_verticesSize;
	std::vector<Index> m_indices;
	UINT m_indicesSize;
	UINT m_indicesCount;

	// Vertex & Index buffer
	Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_indexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
	// Create an upload heap to upload index data to the GPU index buffer heap
	Microsoft::WRL::ComPtr<ID3D12Resource> m_ibUploadHeap;
	// Create an upload heap to upload vertex data to the GPU vertex buffer heap
	Microsoft::WRL::ComPtr<ID3D12Resource> m_vbUploadHeap;

	/**
	* Bundle allocator with which to create the bundle
	*/
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_bundleAllocator;
	/**
	* Bundle to hand to the application that encompasses the drawing of this model
	*/
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_bundle;

	std::string m_name;
};

