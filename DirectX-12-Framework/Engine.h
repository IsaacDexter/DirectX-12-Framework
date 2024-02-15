#pragma once
#include "stdafx.h"
#include <set>
#include <vector>
#include "Renderer.h"

class SceneObject;
class Camera; 
class Window;
class Portal;

class Engine
{
public:
	friend class Renderer;
	Engine(std::shared_ptr<Renderer> renderer, std::shared_ptr<Window> window);
	~Engine();
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
	/**
	* std::array could be useful for contiguous memory with constant buffer views and shader resource views stored acontiguously in memory
	* std::unordered_map and vector are practically the same with UINT
	* You could template this whole class and have it only handle its own type, creating the DescriptorHeap elsewehere taking a start index for that portion of the heap
	* This would make SRVs and CBVs contiguous, but leave a big gap in descriptor heap. Perhaps this is okay though?
	* You could either seperate the class or have one overarching class. This reduces ownership of a single comptr, which is nice, but also sort of makes less sense.
	* It would make reorganising the heap easier though.
	* I think contigous constant buffers is high priority, having a manager would let you iterate over all of them in one, which could be nice. Just Update the view and proj when needed.
	*/
	std::vector<std::shared_ptr<Resource>> m_textures;
	std::vector<std::shared_ptr<Primitive>> m_models;
	std::vector<std::shared_ptr<ConstantBufferView>> m_constantBuffers;
	std::vector<std::shared_ptr<RenderTexture>> m_renderTextures;

	DirectX::XMFLOAT3 CreateRay(int x, int y);
	std::shared_ptr<SceneObject> Pick(const DirectX::XMFLOAT3& rayOrigin, const DirectX::XMFLOAT3& rayDirection);
};

