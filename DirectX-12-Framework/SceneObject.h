#pragma once
#include "stdafx.h"

class Model;
class Texture;
class ConstantBuffer;

class SceneObject
{
public:
	SceneObject(std::shared_ptr<Model> model, std::shared_ptr<Texture> texture, std::shared_ptr<ConstantBuffer> constantBuffer);
	virtual void Initialize(ID3D12Device* device, ID3D12PipelineState* pipelineState, ID3D12RootSignature* rootSignature);
	virtual void Update(const float& deltaTime, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
	void Draw(ID3D12GraphicsCommandList* commandList);

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

	const DirectX::XMMATRIX& GetModel() const
	{
		return DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	}
protected:
	/**
	* Bundle allocator with which to create the bundle
	*/
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_bundleAllocator;
	/** 
	* Bundle to hand to the application that encompasses the drawing of this scene object
	*/
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_bundle;

	DirectX::XMFLOAT3 m_position;

	std::shared_ptr<Model> m_model;
	std::shared_ptr<Texture> m_texture;
	std::shared_ptr<ConstantBuffer> m_constantBuffer;

};

