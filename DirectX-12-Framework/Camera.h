#pragma once
#include "stdafx.h"

class Camera
{
private:
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_direction;
    DirectX::XMFLOAT3 m_up;

    float m_aspectRatio;
    float m_fov;
    float m_nearZ;
    float m_farZ;
    float m_speed;

    bool m_moveLeft;
    bool m_moveRight;
    bool m_moveUp;
    bool m_moveDown;
    bool m_moveForward;
    bool m_moveBackward;

    int m_dX;
    int m_dY;
public:
    Camera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 up, float aspectRatio = 1280 / 720);

    const DirectX::XMMATRIX& GetView();
    const DirectX::XMMATRIX& GetProj();

    /** 
    * Call when resizing the window to resize the camera aspect ratio
    * @param aspectRatio m_window->GetAspectRatio();
    */
    void SetAspectRatio(float aspectRatio)
    {
        m_aspectRatio = aspectRatio;
    }

    void SetPosition(const DirectX::XMFLOAT3& position)
    {
        m_position = position;
    }
    
    void SetDirection(const DirectX::XMFLOAT3& direction)
    {
        m_direction = direction;
    }

    void SetUp(const DirectX::XMFLOAT3& up)
    {
        m_up = up;
    }

    DirectX::XMFLOAT3 GetPosition()
    {
        return m_position;
    }

    void OnKeyDown(WPARAM input);
    void OnKeyUp(WPARAM input);
    
    void MouseInput(int dX, int dY)
    {
        m_dX += dX;
        m_dY += dY;
    }

    void Update(const float& deltaTime);
private:
    void MoveForward(float distance);
    void MoveBackward(float distance)
    {
        MoveForward(-distance);
    }
    void MoveRight(float distance);
    void MoveLeft(float distance)
    {
        MoveRight(-distance);
    }
    void MoveUp(float distance);
    void MoveDown(float distance)
    {
        MoveUp(-distance);
    }
};

