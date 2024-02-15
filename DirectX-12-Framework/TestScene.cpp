#include "TestScene.h"

#include "Camera.h"
#include "Window.h"
#include "Renderer.h"
#include "Portal.h"

#include "SceneObject.h"
#include "Resource.h"
#include "Primitive.h"
#include "ConstantBufferView.h"

using namespace DirectX;

void TestScene::Initialize()
{
    m_camera = std::make_unique<Camera>(XMFLOAT3(0.0f, 1.0f, 4.0f), XMFLOAT3(0.0f, -0.25f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), m_window->GetAspectRatio());

    auto cube = m_renderer->CreateModel(L"Cube", "Cube");
    m_models.push_back(cube);
    auto pyramid = m_renderer->CreateModel(L"Pyramid", "Pyramid");
    m_models.push_back(pyramid);

    auto tiles = m_renderer->CreateTexture(L"Assets/Tiles.dds", "Tiles");
    m_textures.push_back(tiles);
    auto sand = m_renderer->CreateTexture(L"Assets/Sand.dds", "Sand");
    m_textures.push_back(sand);
    auto grass = m_renderer->CreateTexture(L"Assets/Grass.dds", "Grass");
    m_textures.push_back(grass);

    auto Green = m_renderer->CreateTexture(L"Assets/Green.dds", "Green");
    m_textures.push_back(Green);
    auto Blue = m_renderer->CreateTexture(L"Assets/Blue.dds", "Blue");
    m_textures.push_back(Blue);
    auto Red = m_renderer->CreateTexture(L"Assets/Red.dds", "Red");
    m_textures.push_back(Red);
    auto Yellow = m_renderer->CreateTexture(L"Assets/Yellow.dds", "Yellow");
    m_textures.push_back(Yellow);

    // Create the starter objects
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto Floor = SceneObject(cube, grass, cbv, "Floor");
        Floor.SetPosition(XMFLOAT3(0.0f, -0.5f, 0.0f));
        Floor.SetScale(XMFLOAT3(10.0f, 0.0f, 10.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(Floor));
    }
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto wall = SceneObject(cube, Green, cbv, "Green Wall");
        wall.SetPosition(XMFLOAT3(0.0f, 2.0f, -5.0f));
        wall.SetScale(XMFLOAT3(10.0f, 5.0f, 0.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(wall));
    }
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto wall = SceneObject(cube, Red, cbv, "Red Wall");
        wall.SetPosition(XMFLOAT3(-5.0f, 2.0f, 0.0f));
        wall.SetScale(XMFLOAT3(0.0f, 5.0f, 10.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(wall));
    }
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto wall = SceneObject(cube, Blue, cbv, "Blue Wall");
        wall.SetPosition(XMFLOAT3(0.0f, 2.0f, 5.0f));
        wall.SetScale(XMFLOAT3(10.0f, 5.0f, 0.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(wall));
    }
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto wall = SceneObject(cube, Yellow, cbv, "Yellow Wall");
        wall.SetPosition(XMFLOAT3(5.0f, 2.0f, 0.0f));
        wall.SetScale(XMFLOAT3(0.0f, 5.0f, 10.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(wall));
    }
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto Roof = SceneObject(cube, tiles, cbv, "Roof");
        Roof.SetPosition(XMFLOAT3(0.0f, 4.5f, 0.0f));
        Roof.SetScale(XMFLOAT3(10.0f, 0.0f, 10.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(Roof));
    }
    // Create the portals
    {
        auto cbv1 = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv1);
        auto renderTexture1 = m_renderer->CreateRenderTexture("Blue Portal");
        m_renderTextures.push_back(renderTexture1);
        auto portal1 = std::make_shared<Portal>(cube, renderTexture1, cbv1, "Blue Portal", m_sceneObjects, m_camera);
        m_sceneObjects.insert(portal1);
        m_portals.insert(portal1);
        portal1->SetScale(XMFLOAT3(3.0f, 3.0f, 0.0f));
        portal1->SetPosition(XMFLOAT3(0.0f, 1.0f, 4.95f));

        auto cbv2 = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv2);
        auto renderTexture2 = m_renderer->CreateRenderTexture("Green Portal");
        m_renderTextures.push_back(renderTexture2);
        auto portal2 = std::make_shared<Portal>(cube, renderTexture2, cbv2, "Green Portal", m_sceneObjects, m_camera);
        m_sceneObjects.insert(portal2);
        m_portals.insert(portal2);
        portal2->SetScale(XMFLOAT3(3.0f, 3.0f, 0.0f));
        portal2->SetPosition(XMFLOAT3(0.0f, 1.0f, -4.95f));
        portal2->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));


        portal1->SetOtherPortal(portal2);
        portal2->SetOtherPortal(portal1);
    }
    {
        auto cbv1 = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv1);
        auto renderTexture1 = m_renderer->CreateRenderTexture("Red Portal");
        m_renderTextures.push_back(renderTexture1);
        auto portal1 = std::make_shared<Portal>(cube, renderTexture1, cbv1, "Red Portal", m_sceneObjects, m_camera);
        m_sceneObjects.insert(portal1);
        m_portals.insert(portal1);
        portal1->SetScale(XMFLOAT3(3.0f, 3.0f, 0.0f));
        portal1->SetPosition(XMFLOAT3(-4.95f, 1.0f, 0.0f));
        portal1->SetRotation(XMFLOAT3(0.0f, XM_PIDIV2, 0.0f));

        auto cbv2 = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv2);
        auto renderTexture2 = m_renderer->CreateRenderTexture("Yellow Portal");
        m_renderTextures.push_back(renderTexture2);
        auto portal2 = std::make_shared<Portal>(cube, renderTexture2, cbv2, "Yellow Portal", m_sceneObjects, m_camera);
        m_sceneObjects.insert(portal2);
        m_portals.insert(portal2);
        portal2->SetScale(XMFLOAT3(3.0f, 3.0f, 0.0f));
        portal2->SetPosition(XMFLOAT3(4.95f, 1.0f, 0.0f));
        portal2->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
        portal2->SetRotation(XMFLOAT3(0.0f, XM_PIDIV2, 0.0f));


        portal1->SetOtherPortal(portal2);
        portal2->SetOtherPortal(portal1);
    }
}
