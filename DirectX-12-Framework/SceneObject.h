#pragma once
#include "stdafx.h"

class SceneObject
{
public:
	SceneObject();
	virtual void Initialize(ID3D12Device* device, ID3D12PipelineState* pipelineState, ID3D12GraphicsCommandList* commandList, ID3D12RootSignature* rootSignature);
	virtual void Update(const float& deltaTime) {};

	ID3D12GraphicsCommandList* GetBundle()
	{
		return m_bundle.Get();
	}

	const DirectX::XMFLOAT3& GetPosition() const
	{
		return m_position;
	}

	void SetPosition(const DirectX::XMFLOAT3& position)
	{
		m_position = position;
	}

	void SetPosition(const float& x, const float& y, const float& z)
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = z;
	}

	const DirectX::XMMATRIX& GetWorld() const
	{
		return DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	}
protected:
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

	/**
	* Bundle allocator with which to create the bundle
	*/
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_bundleAllocator;
	/** 
	* Bundle to hand to the application that encompasses the drawing of this scene object
	*/
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_bundle;

	DirectX::XMFLOAT3 m_position;
};

