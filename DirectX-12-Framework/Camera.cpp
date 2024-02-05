#include "Camera.h"

using namespace DirectX;

Camera::Camera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 up, float aspectRatio) :
	m_position(position),
	m_direction(direction),
	m_up(up),
	m_fov(0.8f),
	m_aspectRatio(aspectRatio),
	m_nearZ(1.0f),
	m_farZ(1000.0f)
{
	

}

const DirectX::XMMATRIX& Camera::GetView()
{
	return XMMatrixLookToLH(XMLoadFloat3(&m_position), XMLoadFloat3(&m_direction), XMLoadFloat3(&m_up));
}

const DirectX::XMMATRIX& Camera::GetProj()
{
	return XMMatrixPerspectiveFovLH(0.8f, m_aspectRatio, m_nearZ, m_farZ);
}
