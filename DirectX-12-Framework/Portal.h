#pragma once
#include "stdafx.h"
#include <set>
#include <string>
#include "SceneObject.h"

struct RenderTexture;
class Camera;
class Primitive;
struct ConstantBufferView;

class Portal : public SceneObject
{
public:
	Portal(std::shared_ptr<Primitive> model, std::shared_ptr<RenderTexture> renderTexture, std::shared_ptr<ConstantBufferView> constantBuffer, std::string name, std::set<std::shared_ptr<SceneObject>>& objects, std::shared_ptr<Camera>& camera);

	void SetOtherPortal(std::shared_ptr<Portal> otherPortal)
	{
		m_otherPortal = otherPortal;
	}
	
	void DrawTexture(ID3D12GraphicsCommandList* commandList);

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
	std::shared_ptr<Camera> m_camera;
	std::shared_ptr<RenderTexture> m_renderTexture;

	DirectX::XMFLOAT3 m_forward;
	
	std::set<std::shared_ptr<SceneObject>>& g_objects;
	std::shared_ptr<Camera>& g_camera;
};

