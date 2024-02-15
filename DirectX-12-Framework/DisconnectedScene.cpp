#include "DisconnectedScene.h"

#include "Camera.h"
#include "Window.h"
#include "Renderer.h"
#include "Portal.h"

#include "SceneObject.h"
#include "Resource.h"
#include "Primitive.h"
#include "ConstantBufferView.h"

using namespace DirectX;

void DisconnectedScene::Initialize()
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

    auto RedBricks = m_renderer->CreateTexture(L"Assets/RedBricks.dds", "RedBricks");
    m_textures.push_back(RedBricks);
    auto GreenBricks = m_renderer->CreateTexture(L"Assets/GreenBricks.dds", "GreenBricks");
    m_textures.push_back(GreenBricks);

    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto Floor = SceneObject(cube, tiles, cbv, "Middle Floor");
        Floor.SetPosition(XMFLOAT3(0.0f, -0.5f, 0.0f));
        Floor.SetScale(XMFLOAT3(10.0f, 0.0f, 10.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(Floor));
    }

    // Create the First Room objects
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto Floor = SceneObject(cube, sand, cbv, "Red Floor");
        Floor.SetPosition(XMFLOAT3(0.0f, -0.5f, -10.0f));
        Floor.SetScale(XMFLOAT3(10.0f, 0.0f, 10.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(Floor));
    }
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto wall = SceneObject(cube, RedBricks, cbv, "Red North Wall");
        wall.SetPosition(XMFLOAT3(0.0f, 2.0f, -15.0f));
        wall.SetScale(XMFLOAT3(10.0f, 5.0f, 0.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(wall));
    }
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto wall = SceneObject(cube, RedBricks, cbv, "Red East Wall");
        wall.SetPosition(XMFLOAT3(-5.0f, 2.0f, -10.0f));
        wall.SetScale(XMFLOAT3(0.0f, 5.0f, 10.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(wall));
    }
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto wall = SceneObject(cube, RedBricks, cbv, "Red South Wall");
        wall.SetPosition(XMFLOAT3(0.0f, 2.0f, -5.0f));
        wall.SetScale(XMFLOAT3(10.0f, 5.0f, 0.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(wall));
    }
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto wall = SceneObject(cube, RedBricks, cbv, "Red West Wall");
        wall.SetPosition(XMFLOAT3(5.0f, 2.0f, -10.0f));
        wall.SetScale(XMFLOAT3(0.0f, 5.0f, 10.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(wall));
    }
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto Roof = SceneObject(cube, RedBricks, cbv, "Red Roof");
        Roof.SetPosition(XMFLOAT3(0.0f, 4.5f, -10.0f));
        Roof.SetScale(XMFLOAT3(10.0f, 0.0f, 10.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(Roof));
    }

    // Create the Second Room objects
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto Floor = SceneObject(cube, grass, cbv, "Green Floor");
        Floor.SetPosition(XMFLOAT3(0.0f, -0.5f, 10.0f));
        Floor.SetScale(XMFLOAT3(10.0f, 0.0f, 10.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(Floor));
    }
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto wall = SceneObject(cube, GreenBricks, cbv, "Green North Wall");
        wall.SetPosition(XMFLOAT3(0.0f, 2.0f, 5.0f));
        wall.SetScale(XMFLOAT3(10.0f, 5.0f, 0.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(wall));
    }
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto wall = SceneObject(cube, GreenBricks, cbv, "Green East Wall");
        wall.SetPosition(XMFLOAT3(-5.0f, 2.0f, 10.0f));
        wall.SetScale(XMFLOAT3(0.0f, 5.0f, 10.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(wall));
    }
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto wall = SceneObject(cube, GreenBricks, cbv, "Green South Wall");
        wall.SetPosition(XMFLOAT3(0.0f, 2.0f, 15.0f));
        wall.SetScale(XMFLOAT3(10.0f, 5.0f, 0.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(wall));
    }
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto wall = SceneObject(cube, GreenBricks, cbv, "Green West Wall");
        wall.SetPosition(XMFLOAT3(5.0f, 2.0f, 10.0f));
        wall.SetScale(XMFLOAT3(0.0f, 5.0f, 10.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(wall));
    }
    {
        auto cbv = m_renderer->CreateConstantBuffer();
        m_constantBuffers.push_back(cbv);
        auto Roof = SceneObject(cube, GreenBricks, cbv, "Green Roof");
        Roof.SetPosition(XMFLOAT3(0.0f, 4.5f, 10.0f));
        Roof.SetScale(XMFLOAT3(10.0f, 0.0f, 10.0f));
        m_sceneObjects.insert(std::make_shared<SceneObject>(Roof));
    }
    
}
