#pragma once
#include "stdafx.h"
#include <set>
#include "Renderer.h"

class SceneObject;
class Camera; 
class Window;
class Portal;

class Engine
{
public:
	Engine(std::shared_ptr<Renderer> renderer, std::shared_ptr<Window> window);
	virtual void Initialize() = 0;
	virtual void Update();
	void Render();
	void OnKeyDown(WPARAM wParam);
	void OnKeyUp(WPARAM wParam);
	void OnMouseMove(int x, int y, WPARAM wParam);
	void OnResize();
protected:
	std::shared_ptr<Renderer> m_renderer;
	std::shared_ptr<Window> m_window;

	/* TODO :
	Seperate engine and scene properly
	Store textures etc. per scene?
	Add destructor that clears up the in use textures etc.
	*/
	

	std::set<std::shared_ptr<SceneObject>> m_sceneObjects;
	std::set<std::shared_ptr<Portal>> m_portals;
	std::shared_ptr<Camera> m_camera;
	std::shared_ptr<SceneObject> m_selectedObject = nullptr;

	DirectX::XMFLOAT3 CreateRay(int x, int y);
	std::shared_ptr<SceneObject> Pick(const DirectX::XMFLOAT3& rayOrigin, const DirectX::XMFLOAT3& rayDirection);
};

