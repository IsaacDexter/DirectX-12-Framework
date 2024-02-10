#pragma once
#include "stdafx.h"
#include <DirectXCollision.h>

class Primitive;
class Texture;
class ConstantBuffer;

class SceneObject
{
public:
	SceneObject(std::shared_ptr<Primitive> model, std::shared_ptr<Texture> texture, std::shared_ptr<ConstantBuffer> constantBuffer, std::string name);
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
		m_boundingBox.Center = position;
	}

	void SetPosition(const float& x, const float& y, const float& z)
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = z;
		m_boundingBox.Center.x = x;
		m_boundingBox.Center.y = y;
		m_boundingBox.Center.z = z;
	}

	const DirectX::XMFLOAT3& GetRotation() const
	{
		return m_rotation;
	}

	void SetRotation(const DirectX::XMFLOAT3& rotation);
	
	
	void SetRotation(const float& roll, const float& pitch, const float& yaw);
	

	const DirectX::XMFLOAT3& GetScale() const
	{
		return m_scale;
	}

	void SetScale(const DirectX::XMFLOAT3& scale)
	{
		m_scale = scale;
		m_boundingBox.Extents.x = scale.x / 2.0f;
		m_boundingBox.Extents.y = scale.y / 2.0f;
		m_boundingBox.Extents.z = scale.z / 2.0f;
	}
	
	void SetScale(const float& x, const float& y, const float& z)
	{
		m_scale.x = x;
		m_scale.y = y;
		m_scale.z = z;
		m_boundingBox.Extents.x = x / 2.0f;
		m_boundingBox.Extents.y = y / 2.0f;
		m_boundingBox.Extents.z = z / 2.0f;
	}

	const DirectX::XMMATRIX& GetWorld() const;
	

	std::shared_ptr<Primitive> GetModel()
	{
		return m_model;
	}

	void SetModel(std::shared_ptr<Primitive> model)
	{
		m_model = model;
	}

	std::shared_ptr<Texture> GetTexture()
	{
		return m_texture;
	}

	void SetTexture(std::shared_ptr<Texture> texture)
	{
		m_texture = texture;
	}

	DirectX::BoundingOrientedBox& GetBoundingBox()
	{
		return m_boundingBox;
	}

	std::string GetName()
	{
		return m_name;
	}
protected:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_rotation;
	DirectX::XMFLOAT3 m_scale;

	std::string m_name;

	DirectX::BoundingOrientedBox m_boundingBox;

	std::shared_ptr<Primitive> m_model;
	std::shared_ptr<Texture> m_texture;
	std::shared_ptr<ConstantBuffer> m_constantBuffer;

};

