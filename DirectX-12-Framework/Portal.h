#pragma once
#include "stdafx.h"
#include "Resource.h"
#include "SceneObject.h"
#include <set>

class Camera;
class SceneObject;
class Portal;

class Portal : public SceneObject
{
public:
	Portal(ID3D12Device* device, const D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuDescriptorHandle, std::shared_ptr<Primitive> model, std::shared_ptr<ShaderResourceView> texture, std::shared_ptr<ConstantBufferView> constantBuffer, std::string name);

	void SetOtherPortal(std::shared_ptr<Portal> otherPortal)
	{
		m_otherPortal = otherPortal;
	}
	
	void DrawTexture(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, std::set<std::shared_ptr<SceneObject>>& objects);
	
	virtual void SetPosition(const DirectX::XMFLOAT3& position) override;
	virtual void SetRotation(const DirectX::XMFLOAT3& rotation) override;
	virtual void SetScale(const DirectX::XMFLOAT3& scale) override;

	const DirectX::XMMATRIX GetView();
	const DirectX::XMMATRIX GetProj();

protected:
	/** 
	* The portal's 'other side', whose camera will be used to render this portal.
	*/
	std::shared_ptr<Portal> m_otherPortal;
	/** 
	* The portal's camera which is used to determine the mvp matrix when drawing the portal
	*/
	std::unique_ptr<Camera> m_camera;

	const D3D12_CPU_DESCRIPTOR_HANDLE m_rtvCpuDescriptorHandle;
};

