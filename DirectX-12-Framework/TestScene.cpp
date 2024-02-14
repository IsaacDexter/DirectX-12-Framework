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
    {
        m_camera = std::make_unique<Camera>(XMFLOAT3(0.0f, 1.0f, 4.0f), XMFLOAT3(0.0f, -0.25f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), m_window->GetAspectRatio());

        auto cube = m_renderer->CreateModel(L"Cube", "Cube");
        auto pyramid = m_renderer->CreateModel(L"Pyramid", "Pyramid");

        auto tiles = m_renderer->CreateTexture(L"Assets/Tiles.dds", "Tiles");
        auto sand = m_renderer->CreateTexture(L"Assets/Sand.dds", "Sand");

        // Create the starter objects
        {
            auto skybox = SceneObject(cube, tiles, m_renderer->CreateConstantBuffer(), "Skybox");
            skybox.SetPosition(XMFLOAT3(0.0f, 4.5f, 0.0f));
            skybox.SetScale(XMFLOAT3(-10.0f, -10.0f, -10.0f));
            m_sceneObjects.insert(std::make_shared<SceneObject>(skybox));
        }
        {
            m_sceneObjects.insert(std::make_shared<SceneObject>(pyramid, sand, m_renderer->CreateConstantBuffer(), "Pyramid"));
        }
        // Create the portals
        {
            auto renderTexture1 = m_renderer->CreateRenderTexture("Portal1");
            auto portal1 = std::make_shared<Portal>(cube, renderTexture1, m_renderer->CreateConstantBuffer(), "Portal1", m_sceneObjects, m_camera);
            m_sceneObjects.insert(portal1);
            m_portals.insert(portal1);
            portal1->SetScale(XMFLOAT3(1.0f, 1.0f, 0.0f));
            portal1->SetPosition(XMFLOAT3(1.0f, 0.0f, 0.0f));

            auto renderTexture2 = m_renderer->CreateRenderTexture("Portal2");
            auto portal2 = std::make_shared<Portal>(cube, renderTexture2, m_renderer->CreateConstantBuffer(), "Portal2", m_sceneObjects, m_camera);
            m_sceneObjects.insert(portal2);
            m_portals.insert(portal2);
            portal2->SetScale(XMFLOAT3(1.0f, 1.0f, 0.0f));
            portal2->SetPosition(XMFLOAT3(-1.0f, 0.0f, 0.0f));


            portal1->SetOtherPortal(portal2);
            portal2->SetOtherPortal(portal1);
        }
    }
}
