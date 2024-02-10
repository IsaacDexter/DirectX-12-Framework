#include "Engine.h"
#include <DirectXCollision.h>
using namespace DirectX;

Engine::Engine(HINSTANCE hInstance) :
    m_sceneObjects()
{
    m_window = std::make_unique<Window>(hInstance);
    m_renderer = std::make_unique<Renderer>();
    m_renderer->Initialize(m_window->GetHWND(), m_window->GetClientWidth(), m_window->GetClientHeight());
    m_window->Show();
}

void Engine::Initialize()
{
    m_camera = std::make_unique<Camera>(XMFLOAT3(0.0f, 0.0f, 3.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), m_window->GetAspectRatio());
    auto tiles = m_renderer->CreateTexture(L"Assets/Tiles.dds", "tiles");
    auto grass = m_renderer->CreateTexture(L"Assets/Grass.dds", "grass");
    auto cube = m_renderer->CreateModel(L"Assets/Cube.obj", "cube");
    for (UINT i = 0; i < m_numObjects; i++)
    {
        auto object = std::make_shared<SceneObject>(cube, (i % 2 == 0) ? tiles : grass, m_renderer->CreateConstantBuffer(), "Object " + std::to_string(i));
        object->SetPosition(i * 2, 0.0f, 0.0f);
        m_sceneObjects.emplace(object);
    }
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

    // Update constant buffer
    XMMATRIX view = m_camera->GetView();
    XMMATRIX projection = m_camera->GetProj();


    for (auto sceneObject : m_sceneObjects)
    {
        sceneObject->Update(deltaTime, view, projection);
    }
}

void Engine::Render()
{
    m_renderer->Render(m_sceneObjects, m_selectedObject);
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
    case VK_F11:
        m_window->SetFullscreen();
        }
        break;
    case 'T':
        if (m_selectedObject)
            m_selectedObject->SetTexture(m_renderer->CreateTexture(L"Assets/Sand.dds", "sand"));
        break;
    case 'M':
        if (m_selectedObject)
            m_selectedObject->SetModel(m_renderer->CreateModel(L"Assets/Pyramid.obj", "pyramid"));
        break;
    case 'O':
    {
        /*static int count = 1;
        auto object = std::make_shared<SceneObject>(m_renderer->CreateModel(L"Assets/Pyramid.obj"), m_renderer->CreateTexture(L"Assets/Sand.dds"), m_renderer->CreateConstantBuffer(), "New Object " + std::to_string(count));
        object->SetPosition(-count, -count, -count);
        m_sceneObjects.emplace(object);
        count++;*/
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

void Engine::CreateObject()
{
    
}

XMFLOAT3 Engine::CreateRay(int x, int y)
{
    XMMATRIX proj = m_camera->GetProj();
    XMMATRIX view = m_camera->GetView();
    XMMATRIX world = m_camera->GetWorld();

    UINT width = m_window->GetClientWidth();
    UINT height = m_window->GetClientHeight();

    XMVECTOR origin = XMVector3Unproject(XMVectorSet(x, y, 0, 0),
        0,
        0,
        width,
        height,
        0,
        1,
        proj,
        view,
        world
    );

    XMVECTOR dest = XMVector3Unproject(XMVectorSet(x, y, 1, 0),
        0,
        0,
        width,
        height,
        0,
        1,
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
        if (object->GetAABB().Intersects(XMLoadFloat3(&rayOrigin), XMLoadFloat3(&rayDirection), dist))
        {
            return object;
        }
    }

    return nullptr;
}

