#pragma once
#include "stdafx.h"
#include <set>
#include <string>

class SceneObject;
struct RenderTexture;
class Camera;

class Portal
{
public:
	Portal(std::shared_ptr<SceneObject> sceneObject, std::shared_ptr<RenderTexture> renderTexture);

	void SetOtherPortal(std::shared_ptr<Portal> otherPortal)
	{
		m_otherPortal = otherPortal;
	}
	
	void DrawTexture(ID3D12GraphicsCommandList* commandList, std::set<std::shared_ptr<SceneObject>>& objects, std::shared_ptr<Camera> camera);
	
	void SetPosition(const DirectX::XMFLOAT3& position);
	void SetRotation(const DirectX::XMFLOAT3& rotation);
	void SetScale(const DirectX::XMFLOAT3& scale);

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
	std::shared_ptr<SceneObject> m_sceneObject;
	std::shared_ptr<RenderTexture> m_renderTexture;

	DirectX::XMFLOAT3 m_forward;
	
};

