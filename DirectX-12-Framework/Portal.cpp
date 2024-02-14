#include "Portal.h"

#include "SceneObject.h"
#include "RenderTexture.h"

using namespace DirectX;

Portal::Portal(std::shared_ptr<SceneObject> sceneObject, std::shared_ptr<RenderTexture> renderTexture)
	: m_camera(m_sceneObject->GetPosition(), m_forward, XMFLOAT3(0.0f, 1.0f, 0.0f), m_sceneObject->GetScale().x / m_sceneObject->GetScale().y)	// Create the camera from the scene objects position
	, m_forward(0.0f, 0.0f, 1.0f)	// Used in determining camera direction
	, m_renderTexture(renderTexture)
{
	m_sceneObject = sceneObject;	// Store an existing scene object as opposed to creating it. This allows scene objects to be drawn independently of the portal's render texture etc.

	SetRotation(m_sceneObject->GetRotation());	// Update the forward vector according to orientation
	sceneObject->SetTexture(m_renderTexture);	// Replace the scene objects texture with this' render texture
}

void Portal::DrawTexture(ID3D12GraphicsCommandList* commandList, std::set<std::shared_ptr<SceneObject>>& objects, std::shared_ptr<Camera> camera)
{
	m_renderTexture->BeginDraw(commandList);
	for (auto object : objects)
	{
		if (object != m_sceneObject)
		{
			object->UpdateConstantBuffer(m_camera.GetView(), m_camera.GetProj());
			object->Draw(commandList);
		}
	}
	m_renderTexture->EndDraw(commandList);
}

void Portal::SetPosition(const DirectX::XMFLOAT3& position)
{
	m_sceneObject->SetPosition(position);
	m_camera.SetPosition(position);
}

void Portal::SetRotation(const DirectX::XMFLOAT3& rotation)
{
	m_sceneObject->SetRotation(rotation);
	// TODO : Update camera direction according to rotation
	// To get the direction the camera should be in:
	// * find the direction from the player camera to this portal?
	// * this will give the illusion of the portal being a seamless part of the world

	// Get the portal's forward, i.e. where its facing 
	auto forward = XMLoadFloat3(&m_forward);
	auto qRotation = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));
	// Rotate the portal's forward according to the rotation
	auto newForward = XMVector3Rotate(forward, qRotation);
	newForward = XMVector3Normalize(newForward);
	XMStoreFloat3(&m_forward, newForward);
	m_camera.SetDirection(m_forward);

}

void Portal::SetScale(const DirectX::XMFLOAT3& scale)
{
	m_sceneObject->SetScale(scale);
	m_camera.SetAspectRatio(scale.x / scale.y);
}


const DirectX::XMMATRIX Portal::GetView()
{
	return m_camera.GetView();
}

const DirectX::XMMATRIX Portal::GetProj()
{
	return m_camera.GetProj();
}
