#pragma once
#include "stdafx.h"
#include "Camera.h"
#include "Resource.h"

class Portal
{
public:
	Portal();
	void SetOtherPortal(std::shared_ptr<Portal> otherPortal)
	{
		m_otherPortal = otherPortal;
	}
	std::shared_ptr<Camera> GetCamera()
	{

	}

private:
	/** 
	* The portal's 'other side', whose camera will be used to render this portal.
	*/
	std::shared_ptr<Portal> m_otherPortal;
	/** 
	* The portal's camera which is used to determine the mvp matrix when drawing the portal
	*/
	std::shared_ptr<Camera> m_camera;

	/** 
	* Resources and handles for the RenderTexture RTV/SRV
	*/
	Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTexture;
	const ResourceHandle m_srvHandle;
	const ResourceHandle m_rtvHandle;
};

