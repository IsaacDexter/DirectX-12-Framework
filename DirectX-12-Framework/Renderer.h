#pragma once
#include "stdafx.h"
#include <array>
#include <set>

#include "CommandQueue.h"
#include "DescriptorHeap.h"
#include "CbvSrvUavHeap.h"


class Camera;
class Primitive;
struct ConstantBufferView;
struct ShaderResourceView;
struct RenderTexture;
struct Resource;
class Portal;
class SceneObject;
class Engine;


class Renderer
{
public:
	Renderer(std::shared_ptr<Engine>& scene);
	std::shared_ptr<Engine>& g_scene;
	/*
	1. Initialize
	2. Repeat
		- Update
		- Render
	3. Destroy
	*/
	void Initialize(HWND hWnd, const UINT width, const UINT height);
	void Update();
	void Render();
	void Destroy();
	void Resize(const UINT width, const UINT height);

	std::shared_ptr<Resource> CreateTexture(const wchar_t* path, std::string name);
	std::shared_ptr<Resource> CreateTexture(std::string name);
	std::shared_ptr<RenderTexture> CreateRenderTexture(std::string name);
	std::shared_ptr<Primitive> CreateModel(const wchar_t* path, std::string name);
	std::shared_ptr<ConstantBufferView> CreateConstantBuffer();

	void UnloadResource(D3D12_CPU_DESCRIPTOR_HANDLE cbvSrvUavCpuDescriptorHandle, D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvUavGpuDescriptorHandle);
	void UnloadResource(D3D12_CPU_DESCRIPTOR_HANDLE cbvSrvUavCpuDescriptorHandle, D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvUavGpuDescriptorHandle, D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuDescriptorHandle);


private:
	static const UINT m_frameCount = 2;

#pragma region Pipeline

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device4> m_device;
	// I think this should be called m_frameBuffers, but wikipedia says its just one word. Go figure.
	std::array<std::pair<Microsoft::WRL::ComPtr<ID3D12Resource>, D3D12_CPU_DESCRIPTOR_HANDLE>, Renderer::m_frameCount> m_framebuffers;
	UINT m_frameIndex;

	
	Microsoft::WRL::ComPtr<ID3D12Resource> m_dsv;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_samplerHeap;


	bool m_useWarpDevice = false;

#pragma endregion

#pragma region Resources


	std::unique_ptr<CbvSrvUavHeap> m_cbvSrvUavHeap;
	std::unique_ptr<DescriptorHeap> m_rtvHeap;

#pragma endregion

#pragma region Sync

	std::unique_ptr<CommandQueue> m_commandQueue;

#pragma endregion

private:
#pragma region Initialization

	void InitializePipeline(HWND hWnd, const UINT width, const UINT height);

	Microsoft::WRL::ComPtr<ID3D12Debug> EnableDebugLayer();
	/**
	* Compatible adapter must be present before creating D3D12 device
	* @param useWarpDevice Whether or not to use software rasterizer
	* @returns pointer to the adapter
	*/
	Microsoft::WRL::ComPtr<IDXGIAdapter4> GetAdapter(bool useWarpDevice);

	void GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter);
	/**
	* Create D3D12 device, used to create resources
	* @param Adapter from GetAdapter
	* @returns ComPtr to newly created D3D12 Device
	*/
	Microsoft::WRL::ComPtr<ID3D12Device4> CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter);

	void CreateDevice();
	/**
	* Desribe and create a command queue
	* @param type the type of command queue to create
	* @returns The newly created command queue
	*/
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> CreateCommandQueue(Microsoft::WRL::ComPtr<ID3D12Device> device, const D3D12_COMMAND_LIST_TYPE type);

	/**
	* Create the dxgi factory and swap chain, used to flip the front and back buffer
	* @param hWnd Handle to OS window used to display rendered image
	* @param commandQueue command queue used to issue commands to the cpu
	* @param describes the resolution width, set to 0 to obtain that of the output window
	* @param describes the resolution height, set to 0 to obtain that of the output window
	* @param the number of buffers in the swap chain
	* @returns ComPtr to the newly created swap chain
	*/
	Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateSwapChain(HWND hWnd, Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, uint32_t width, uint32_t height, uint32_t bufferCount);

	/**
	* Create the descriptor heap, an array of resource views.Used to store render target views for the swap chain buffers
	* @param device comptr to the dx12 device
	* @param type Type that specifies the types of descriptors in the heap: CVB, SRV, UAV; Sampler; RTV; DSV
	* @param numDescriptors Number of descriptors in the heap
	* @returns ComPtr to newly created descriptor heap
	*/
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device4> device, const D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors, UINT &descriptorSize);

	void CreateFramebuffers();
	void UpdateFramebuffers();

	/**
	* Check if screen tearing should be allowed for variable refresh displays
	* @returns true if tearing should be allowed
	*/
	bool CheckTearingSupport();

	void InitializeAssets(const UINT width, const UINT height);

	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRootSignature();

	void CreateSyncObjects();

	void UpdateDepthStencilView(Microsoft::WRL::ComPtr<ID3D12Resource> &dsv, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& dsvHeap, UINT width, UINT height);

	
	
	void CreateSampler();

	/** 
	* Creates the pipeline state stream, serialized object, and object. 
	* The various descs are pulled from their various functions.
	* @param pVertexShaderBlob pointer to memory block containing vertex shader
	* @param pPixelShaderBlob pointer to memory block containing pixel shader
	* @returns ComPtr to the created PSO
	*/
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreatePipelineStateObject(ID3DBlob* pVertexShaderBlob, ID3DBlob* pPixelShaderBlob);

	void InitializeGUI(HWND hWnd);
	void UpdateGUI(std::set<std::shared_ptr<SceneObject>>& objects, std::shared_ptr<SceneObject>& selectedObject);
	void ShowSceneGraph(std::set<std::shared_ptr<SceneObject>>& objects, std::shared_ptr<SceneObject>& selectedObject);
	void ShowProperties(std::shared_ptr<SceneObject>& selectedObject);
	void DestroyGUI();
	void RenderGUI(ID3D12GraphicsCommandList* commandList);


#pragma endregion

#pragma region Rendering

	void PrepareCommandList(ID3D12GraphicsCommandList* commandList);
	
#pragma endregion
};

