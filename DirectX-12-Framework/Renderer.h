#pragma once
#include "stdafx.h"
#include <array>
#include <vector>
#include "DDSTextureLoader.h"
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include <chrono>
#include <set>

#include "Camera.h"
#include "Controls.h"
#include "Primitive.h"
#include "Texture.h"
#include "ConstantBuffer.h"
#include "CbvSrvUavHeap.h"
#include "SceneObject.h"

class Renderer
{
public:
	Renderer();
	
	/*
	1. Initialize
	2. Repeat
		- Update
		- Render
	3. Destroy
	*/
	void Initialize(HWND hWnd, UINT width, UINT height);
	void Update();
	void Render(std::set<std::shared_ptr<SceneObject>>& objects, std::shared_ptr<SceneObject>& selectedObject);
	void Destroy();
	void Resize(UINT width, UINT height);

	std::shared_ptr<Texture> CreateTexture(const wchar_t* path, std::string name);
	std::shared_ptr<Primitive> CreateModel(const wchar_t* path, std::string name);
	std::shared_ptr<ConstantBuffer> CreateConstantBuffer();

private:
	static const UINT m_frameCount = 2;

#pragma region Pipeline

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device4> m_device;
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, Renderer::m_frameCount> m_renderTargets;
	std::array<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, Renderer::m_frameCount > m_commandAllocators;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_dsv;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	UINT m_rtvDescriptorSize;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_samplerHeap;


	bool m_useWarpDevice = false;

#pragma endregion

#pragma region Resources


	std::unique_ptr<CbvSrvUavHeap> m_cbvSrvUavHeap;
	

#pragma endregion

#pragma region Sync

	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
	std::array<UINT64, Renderer::m_frameCount> m_fenceValues;

#pragma endregion

private:
#pragma region Initialization

	void InitializePipeline(HWND hWnd, UINT width, UINT height);

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

	void UpdateRenderTargetViews(Microsoft::WRL::ComPtr<ID3D12Device4> device, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap, Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain, std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, Renderer::m_frameCount>& renderTargets);

	/**
	* Check if screen tearing should be allowed for variable refresh displays
	* @returns true if tearing should be allowed
	*/
	bool CheckTearingSupport();

	void InitializeAssets();

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

	void MoveToNextFrame();
	void WaitForGpu();

#pragma endregion

#pragma region Rendering

	void PopulateCommandList(std::set<std::shared_ptr<SceneObject>>& objects);

#pragma endregion
};

