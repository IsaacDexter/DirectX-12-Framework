#include "Engine.h"
#include <DirectXCollision.h>
#include <string>
#include <chrono>

#include "Camera.h"
#include "Window.h"
#include "Renderer.h"
#include "Portal.h"

#include "SceneObject.h"
#include "Resource.h"
#include "Primitive.h"
#include "ConstantBufferView.h"

using namespace DirectX;


Engine::Engine(std::shared_ptr<Renderer> renderer, std::shared_ptr<Window> window)
    : m_sceneObjects()
    , m_renderer(renderer)
    , m_window(window)
{

}


void Engine::Update()
{
    // Log FPS

        // Set up timers and frame counters
        ///<summary>number of times a frame was rendered to the screen since last frameRate was outputted</summary>
    static uint64_t frameCounter = 0;
    ///<summary>number of seconds passed since last frameRate was outputted</summary>
    static double elapsedSeconds = 0.0;
    ///<summary>clock used to sample time points</summary>
    static std::chrono::high_resolution_clock clock;
    ///<summary>initial point in time, the time at the last frame</summary>
    static auto timeLast = clock.now();

    // calculate the new deltaTime
    frameCounter++;
    auto timeNow = clock.now();
    auto timeSince = timeNow - timeLast;
    timeLast = timeNow;

    // add the delta time, in seconds, since the last frame
    auto deltaTime = timeSince.count() * 1e-9;
    elapsedSeconds += deltaTime;

    // output the FPS
    // if it's been a second...
    if (elapsedSeconds > 10.0)
    {

        auto fps = frameCounter / elapsedSeconds;
        char buffer[500];
        sprintf_s(buffer, 500, "FPS: %f, DT: %f\n", fps, deltaTime);
        OutputDebugStringA(buffer);

        // reset counters
        frameCounter = 0;
        elapsedSeconds = 0.0;
    }


    // Update the camera based on input previously passed to it.
    m_camera->Update(deltaTime);

    // Update scene objects
    for (auto sceneObject : m_sceneObjects)
    {
        sceneObject->Update(deltaTime);
    }
}

void Engine::Render()
{
    // Update constant buffer
    m_renderer->Render(m_sceneObjects, m_portals, m_selectedObject, m_camera);
}

void Engine::OnKeyDown(WPARAM wParam)
{
    bool alt = (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

    switch (wParam)
    {
        // Quit
    case VK_ESCAPE:
    {
        ::PostQuitMessage(0);
        m_renderer->Destroy();
        break;
    }
    // Toggle Fullscreen
    case VK_RETURN:
        if (alt)
        {
            [[fallthrough]];
    case VK_F11:
        m_window->SetFullscreen();
        }
        break;
    default:
        break;
    }
    m_camera->OnKeyDown(wParam);
}

void Engine::OnKeyUp(WPARAM wParam)
{
    m_camera->OnKeyUp(wParam);
}

void Engine::OnMouseMove(int x, int y, WPARAM wParam)
{
    static auto lastX = x;
    static auto lastY = y;
    auto dX = x - lastX;
    auto dY = y - lastY;
    lastX = x;
    lastY = y;

    switch (wParam)
    {
    case MK_LBUTTON:
    {
        m_camera->OnMouseMove(dX, dY, wParam);
    }
    break;
    case MK_RBUTTON:
    {
        m_selectedObject = Pick(m_camera->GetPosition(), CreateRay(x, y));
    }
    break;
    default:
    {
        break;
    }
    }

    
}

void Engine::OnResize()
{
    // Check if the size actually changed
    if (m_window->Resize())
    {
        auto width = m_window->GetClientWidth();
        auto height = m_window->GetClientHeight();
        m_camera->SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
        m_renderer->Resize(width, height);
    }
}

XMFLOAT3 Engine::CreateRay(int x, int y)
{
    XMMATRIX proj = m_camera->GetProj();
    XMMATRIX view = m_camera->GetView();
    XMMATRIX world = m_camera->GetWorld();

    float width = static_cast<float>(m_window->GetClientWidth());
    float height = static_cast<float>(m_window->GetClientHeight());

    XMVECTOR origin = XMVector3Unproject(XMVectorSet(static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f),
        0.0f,
        0.0f,
        width,
        height,
        0.0f,
        1.0f,
        proj,
        view,
        world
    );

    XMVECTOR dest = XMVector3Unproject(XMVectorSet(static_cast<float>(x), static_cast<float>(y), 1.0f, 0.0f),
        0.0f,
        0.0f,
        width,
        height,
        0.0f,
        1.0f,
        proj,
        view,
        world
    );

    XMVECTOR direction = dest - origin;
    direction = XMVector3Normalize(direction);
    XMFLOAT3 ray;
    XMStoreFloat3(&ray, direction);

    
    return ray;
}

std::shared_ptr<SceneObject> Engine::Pick(const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDirection)
{
    //char buffer[500];
    //sprintf_s(buffer, 500, "Direction: (%f, %f, %f)\nOrigin: (%f, %f, %f)\n", rayDirection.x, rayDirection.y, rayDirection.z, rayOrigin.x, rayOrigin.y, rayOrigin.z);
    //OutputDebugStringA(buffer);
    for (auto object : m_sceneObjects)
    {
        float dist;
        if (object->GetBoundingBox().Intersects(XMLoadFloat3(&rayOrigin), XMLoadFloat3(&rayDirection), dist))
        {
            return object;
        }
    }

    return nullptr;
}

