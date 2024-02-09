#include "Engine.h"
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
    auto tiles = m_renderer->CreateTexture(L"Assets/Tiles.dds");
    auto grass = m_renderer->CreateTexture(L"Assets/Grass.dds");
    auto cube = m_renderer->CreateModel(L"Assets/Cube.obj");
    for (UINT i = 0; i < m_numObjects; i++)
    {
        std::string name = "Object " + std::to_string(i);
        auto object = SceneObject(cube, (i % 2 == 0) ? tiles : grass, m_renderer->CreateConstantBuffer());
        object.SetPosition(i * 2, 0.0f, 0.0f);
        m_sceneObjects.try_emplace(name, object);
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
        sceneObject.second.Update(deltaTime, view, projection);
    }
}

void Engine::Render()
{
    m_renderer->Render(m_sceneObjects);
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
        m_sceneObjects.at("Object 1").SetTexture(m_renderer->CreateTexture(L"Assets/Sand.dds"));
        break;
    case 'M':
        m_sceneObjects.at("Object 0").SetModel(m_renderer->CreateModel(L"Assets/Pyramid.obj"));
        break;
    case 'O':
    {
        static int count = 1;
        std::string name = "NewObject " + std::to_string(count);
        auto object = SceneObject(m_renderer->CreateModel(L"Assets/Pyramid.obj"), m_renderer->CreateTexture(L"Assets/Sand.dds"), m_renderer->CreateConstantBuffer());
        object.SetPosition(-count, -count, -count);
        m_sceneObjects.try_emplace(name, object);
        count++;
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

void Engine::OnMouseMove(int dX, int dY, WPARAM wParam)
{
    m_camera->OnMouseMove(dX, dY, wParam);
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
