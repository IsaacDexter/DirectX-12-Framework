#pragma once
#include "stdafx.h"
#include "Resource.h"
#include <set>

class Camera;
class SceneObject;
class Portal;

class Portal
{
public:
	Portal(ID3D12Device* device, const ResourceHandle rtvHandle, const ResourceHandle srvHandle);
	void SetOtherPortal(std::shared_ptr<Portal> otherPortal)
	{
		m_otherPortal = otherPortal;
	}
	const DirectX::XMMATRIX& GetView();
	
	const DirectX::XMMATRIX& GetProj();
	
	void Draw(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, std::set<std::shared_ptr<SceneObject>>& objects);
	

private:
	/** 
	* The portal's 'other side', whose camera will be used to render this portal.
	*/
	std::shared_ptr<Portal> m_otherPortal;
	/** 
	* The portal's camera which is used to determine the mvp matrix when drawing the portal
	*/
	std::unique_ptr<Camera> m_camera;

	/** 
	* Resources and handles for the RenderTexture RTV/SRV
	*/
	Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTexture;
	const ResourceHandle m_srvHandle;
	const ResourceHandle m_rtvHandle;
};

