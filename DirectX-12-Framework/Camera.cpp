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
	m_dX(0),
	m_dY(0),
	m_moveRight(false),
	m_moveLeft(false),
	m_moveUp(false),
	m_moveDown(false),
	m_moveForward(false),
	m_moveBackward(false)
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

void Camera::OnKeyDown(WPARAM input)
{
	// Update Based on keyboard input since last frame:
	switch (input)
	{
	case Controls::MoveRight:
	{
		m_moveRight = true;
		break;
	}
	case Controls::MoveLeft:
	{
		m_moveLeft = true;
		break;
	}
	case Controls::MoveUp:
	{
		m_moveUp = true;
		break;
	}
	case Controls::MoveDown:
	{
		m_moveDown = true;
		break;
	}
	case Controls::MoveForward:
	{
		m_moveForward = true;
		break;
	}
	case Controls::MoveBackward:
	{
		m_moveBackward = true;
		break;
	}
	default:
		break;
	}
}

void Camera::OnKeyUp(WPARAM input)
{
	// Update Based on keyboard input since last frame:
	switch (input)
	{
	case Controls::MoveRight:
	{
		m_moveRight = false;
		break;
	}
	case Controls::MoveLeft:
	{
		m_moveLeft = false;
		break;
	}
	case Controls::MoveUp:
	{
		m_moveUp = false;
		break;
	}
	case Controls::MoveDown:
	{
		m_moveDown = false;
		break;
	}
	case Controls::MoveForward:
	{
		m_moveForward = false;
		break;
	}
	case Controls::MoveBackward:
	{
		m_moveBackward = false;
		break;
	}
	default:
		break;
	}
}

void Camera::Update(const float& deltaTime)
{
	// Move according to previously recieved keybard input
	{
		if(m_moveRight)
			MoveRight(m_speed * deltaTime);
		if(m_moveLeft)
			MoveLeft(m_speed * deltaTime);
		if(m_moveUp)
			MoveUp(m_speed * deltaTime);
		if(m_moveDown)
			MoveDown(m_speed * deltaTime);
		if(m_moveForward)
			MoveForward(m_speed * deltaTime);
		if(m_moveBackward)
			MoveBackward(m_speed * deltaTime);
	}





	// Update based on mouse input, if input has changed
	{
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
			//XMStoreFloat3(&m_up, upVec);
		}
	}


	// Reset the change in mose motion each frame
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
