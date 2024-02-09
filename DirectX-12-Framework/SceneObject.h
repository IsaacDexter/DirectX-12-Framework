#pragma once
#include "stdafx.h"

class Primitive;
class Texture;
class ConstantBuffer;

class SceneObject
{
public:
	SceneObject(std::shared_ptr<Primitive> model, std::shared_ptr<Texture> texture, std::shared_ptr<ConstantBuffer> constantBuffer);
	virtual void Initialize();
	virtual void Update(const float& deltaTime, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
	void Draw(ID3D12GraphicsCommandList* commandList);

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

	void SetModel(std::shared_ptr<Primitive> model)
	{
		m_model = model;
	}
	
	void SetTexture(std::shared_ptr<Texture> texture)
	{
		m_texture = texture;
	}
protected:
	DirectX::XMFLOAT3 m_position;

	std::shared_ptr<Primitive> m_model;
	std::shared_ptr<Texture> m_texture;
	std::shared_ptr<ConstantBuffer> m_constantBuffer;

};

