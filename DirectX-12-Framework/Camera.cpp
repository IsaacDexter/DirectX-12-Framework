#include "Camera.h"
#include "Controls.h"

using namespace DirectX;

Camera::Camera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 up, float aspectRatio) :
	m_position(position),
	m_direction(direction),
	m_up(up),
	m_fov(0.8f),
	m_aspectRatio(aspectRatio),
	m_nearZ(1.0f),
	m_farZ(1000.0f),
	m_speed(5.0f),
	m_input(NULL),
	m_dX(0),
	m_dY(0)
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

void Camera::Update(const float& deltaTime)
{
	// Update Based on keyboard input since last frame:
	switch (m_input)
	{
	case Controls::MoveRight:
	{
		MoveRight(m_speed * deltaTime);
		break;
	}
	case Controls::MoveLeft:
	{
		MoveLeft(m_speed * deltaTime);
		break;
	}
	case Controls::MoveUp:
	{
		MoveUp(m_speed * deltaTime);
		break;
	}
	case Controls::MoveDown:
	{
		MoveDown(m_speed * deltaTime);
		break;
	}
	case Controls::MoveForward:
	{
		MoveForward(m_speed * deltaTime);
		break;
	}
	case Controls::MoveBackward:
	{
		MoveBackward(m_speed * deltaTime);
		break;
	}
	default:
		break;
	}

	// Update based on mouse input, if input has changed
	if (m_dX != 0 || m_dY != 0)
	{
		// Apply sensitivity
		float dX = m_dX * Controls::sensitivity; // Yaw change
		float dY = m_dY * Controls::sensitivity; // Pitch change


		// Get the current look direction and up vector
		XMVECTOR lookDirVec = XMLoadFloat3(&m_direction);
		lookDirVec = XMVector3Normalize(lookDirVec);
		XMVECTOR upVec = XMLoadFloat3(&m_up);
		upVec = XMVector3Normalize(upVec);

		// Calculate the camera's right vector
		XMVECTOR rightVec = XMVector3Cross(upVec, lookDirVec);
		rightVec = XMVector3Normalize(rightVec);



		// Rotate the lookDir vector left or right based on the yaw
		lookDirVec = XMVector3Transform(lookDirVec, XMMatrixRotationAxis(upVec, dX));
		lookDirVec = XMVector3Normalize(lookDirVec);

		// Rotate the lookDir vector up or down based on the pitch
		lookDirVec = XMVector3Transform(lookDirVec, XMMatrixRotationAxis(rightVec, dY));
		lookDirVec = XMVector3Normalize(lookDirVec);


		// Re-calculate the right vector after the yaw rotation
		rightVec = XMVector3Cross(upVec, lookDirVec);
		rightVec = XMVector3Normalize(rightVec);

		// Re-orthogonalize the up vector to be perpendicular to the look direction and right vector
		upVec = XMVector3Cross(lookDirVec, rightVec);
		upVec = XMVector3Normalize(upVec);

		// Store the updated vectors back to the class members
		XMStoreFloat3(&m_direction, lookDirVec);
		XMStoreFloat3(&m_up, upVec);
	}


	// remove the input to prevent duplicate inputs. It's a rather rudimentary camera.
	m_input = NULL;
	m_dX = 0;
	m_dY = 0;
}

void Camera::MoveForward(float distance)
{
	// Get the normalized forward vector (camera's look direction)
	XMVECTOR forwardVec = XMVector3Normalize(XMLoadFloat3(&m_direction));
	XMVECTOR posVec = XMLoadFloat3(&m_position);

	// Move in the direction the camera is facing
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(XMVectorReplicate(distance), forwardVec, posVec));
}

void Camera::MoveRight(float distance)
{
	// Get the current look direction and up vector
	XMVECTOR lookDirVec = XMLoadFloat3(&m_direction);
	XMVECTOR upVec = XMLoadFloat3(&m_up);

	// Calculate the right vector (side vector of the camera)
	XMVECTOR rightVec = XMVector3Normalize(XMVector3Cross(upVec, lookDirVec));
	XMVECTOR posVec = XMLoadFloat3(&m_position);

	// Move left by moving opposite to the right vector
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(XMVectorReplicate(distance), rightVec, posVec));
}

void Camera::MoveUp(float distance)
{
	// Get the normalized forward vector (camera's look direction)
	XMVECTOR upVec = XMVector3Normalize(XMLoadFloat3(&m_up));
	XMVECTOR posVec = XMLoadFloat3(&m_position);

	// Move in the direction the camera is facing
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(XMVectorReplicate(distance), upVec, posVec));
}
