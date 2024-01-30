#pragma once
#include "stdafx.h"

class Application
{
public:
	Application(UINT width, UINT height, const wchar_t* name);
	
	/*
	1. Initialize
	2. Repeat
		- Update
		- Render
	3. Destroy
	*/
	void Initialize();
	void Update();
	void Render();
	void Destroy();

private:
#pragma region Initialization

	void InitializePipeline();

	Microsoft::WRL::ComPtr<ID3D12Debug> EnableDebugLayer();
	/** 
	* Compatible adapter must be present before creating D3D12 device
	* @param useWarpDevice Whether or not to use software rasterizer
	* @returns pointer to the adapter
	*/
	Microsoft::WRL::ComPtr<IDXGIAdapter4> GetAdapter(bool useWarpDevice);
	/** 
	* Create D3D12 device, used to create resources
	* @param Adapter from GetAdapter
	* @returns ComPtr to newly created D3D12 Device
	*/
	Microsoft::WRL::ComPtr<ID3D12Device3> CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter);

	/** 
	* Desribe and create a command queue
	* @param type the type of command queue to create
	* @returns The newly created command queue
	*/
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> CreateCommandQueue(Microsoft::WRL::ComPtr<ID3D12Device> device, const D3D12_COMMAND_LIST_TYPE type);


	void InitializeAssets();

#pragma endregion

#pragma region Rendering

	void PopulateCommandList();
	void WaitForPreviousFrame();

#pragma endregion





	static const UINT m_frameCount = 2;

#pragma region Pipeline

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device3> m_device;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[m_frameCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
	UINT m_rtvDescriptorSize;
	bool m_useWarpDevice = false;

#pragma endregion

#pragma region Resources

	Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

#pragma endregion

#pragma region Sync

	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;

#pragma endregion

};

