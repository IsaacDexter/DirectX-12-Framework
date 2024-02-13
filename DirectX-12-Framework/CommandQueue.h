#pragma once
#include "stdafx.h"
#include <queue>
#include <chrono>

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

class CommandQueue
{
private:
	/// <summary><para>associates a fence value with a command allocator</para>
	/// <para>command allocators cannot be reused until the commands stored in the allocator have finished execution on the command queue</para>
	/// <para>a fence value is signalled on the command queue and is stored for later use</para></summary>
	struct CommandAllocatorEntry
	{
		uint64_t fenceValue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	};

	/// <summary><para>queue of allocators currently in-flight on the GPU queue.</para>
	/// <para>as soon as the fence value associated to each entry has been reached, the command allocator can be reused.</para></summary>
	std::queue<CommandAllocatorEntry> m_commandAllocatorQueue;
	/// <summary>queue of command lists that can be reused, as the can be reused immediately after execution</summary>
	std::queue<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> m_commandListQueue;

	/// <summary>type of d3d12 command queue</summary>
	const D3D12_COMMAND_LIST_TYPE m_commandListType;
	/// <summary>device, used to create command queue, lists, and allocators</summary>
	const Microsoft::WRL::ComPtr<ID3D12Device> m_device;
	/// <summary>command queue owned by this class</summary>
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;

	/// <summary>Used to synchronize commands issued to command queue</summary>
	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
	/// <summary>Handle to OS event used to recieve notification that fences have reached a certain value</summary>
	HANDLE m_fenceEvent;
	/// <summary>Next fence value to signal the command queue next</summary>
	uint64_t m_fenceValue;

public:
	/// <summary>create the command queue</summary>
	/// <param name="device">d3d device</param>
	/// <param name="type">The type of cmmand queue to create</param>
	CommandQueue(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type);
	virtual ~CommandQueue();

	/// <summary></summary>
	/// <returns>a command list immediately ready to issue commands, no need to reset or create a command allocator</returns>
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList(ID3D12PipelineState* initialState);


	/// <summary>executes the commands stored into a command list from GetCommandList()</summary>
	/// <param name="commandList">command list from GetCommandList(), with commands stored in it</param>
	/// <returns>a fence value that can be used to check if/wait until the commands have finished executing</returns>
	uint64_t ExecuteCommandList(ID3D12GraphicsCommandList* commandList);


	/// <summary>Signal the fence from the GPU, done after all commands on the queue have finished executing</summary>
	/// <returns>the fence value that the CPU thread should wait for before reusing any in-flight resources</returns>
	uint64_t Signal();
	/// <summary>Wait until the fence is signaled with a particular value, stalling the CPU thread until the GPU queue has finished executing commands</summary>
	void WaitForFenceValue(uint64_t fenceValue);
	/// <summary>Wait until all previous commands have finished executing. Ensures back buffer resources have finsihed executing. A Singal followed by WaitForFenceValue</summary>
	void Flush();
	/// <summary>Checks if a certain fence value is reached at this current point in time</summary>
	/// <param name="fenceValue">the fence value to check</param>
	/// <returns>true if this fence value has been reached</returns>
	bool IsFenceComplete(uint64_t fenceValue);

	/// <summary></summary>
	/// <returns>Underlying ID3D12CommandQueue interface</returns>
	ID3D12CommandQueue* const GetD3D12CommandQueue()
	{
		return m_commandQueue.Get();
	}

protected:
	/// <summary>create the command allocator, used by a command list as backing memory</summary>
	/// <returns>comptr to newly created CA</returns>
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();
	/// <summary>Create a command list, which records commands executed on the GPU</summary>
	/// <returns>comptr to the newly created CL</returns>
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CreateCommandList(ID3D12CommandAllocator* allocator, ID3D12PipelineState* pipelineState);



	/// <summary>Create the command queue used to issue commands to the cpu</summary>
	/// <param name="device">comptr to the dx12 device</param>
	/// <param name="type">copy, direct or compute</param>
	/// <returns>comptr to the created command queue</returns>
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> CreateCommandQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
	/// <summary>Creates a fence, a GPU/CPU sync object. stores an internal value that is udated after singalling, used to wait for events</summary>
	/// <param name="device">the d3d12 device</param>
	/// <returns>comptr to the newly created fence</returns>
	Microsoft::WRL::ComPtr < ID3D12Fence> CreateFence(ID3D12Device* device);
	/// <summary>create the command allocator, used by a command list as backing memory</summary>
	/// <param name="device">the d3d12 device</param>
	/// <param name="type">a command list type specifying if it records either direct command lists or bundles: DIRECT, BUNDLE, COMPUTE, COPY</param>
	/// <returns>comptr to newly created CA</returns>
	Microsoft::WRL::ComPtr < ID3D12CommandAllocator> CreateCommandAllocator(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
	/// <summary>Create a command list, which records commands executed on the GPU</summary>
	/// <param name="device">the d3d12 device</param>
	/// <param name="commandAllocator">command allocator to create the command list from</param>
	/// <param name="type">the type: DIRECT, BUNDLE, COMPUTE, COPY</param>
	/// <returns>comptr to the newly created CL</returns>
	Microsoft::WRL::ComPtr < ID3D12GraphicsCommandList> CreateCommandList(ID3D12Device* device, ID3D12CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState, D3D12_COMMAND_LIST_TYPE type);
	/// <summary>Create an OS event handle, used to allow the CPU thread to wait (block) for fence signalling</summary>
	/// <returns>A handle to the os event</returns>
	HANDLE CreateEventHandle();



	/// <summary>Signal the fence from the GPU, done after all commands on the queue have finished executing</summary>
	/// <param name="commandQueue">THe command queue to signal</param>
	/// <param name="fence">The fence to be singalled</param>
	/// <param name="fenceValue">value to singal the fence with</param>
	/// <returns>the fence value that the CPU thread should wait for before reusing any in-flight resources</returns>
	uint64_t Signal(ID3D12CommandQueue* commandQueue, ID3D12Fence* fence, uint64_t& fenceValue);
	/// <summary>Wait until the fence is signaled with a particular value, stalling the CPU thread until the GPU queue has finished executing commands</summary>
	/// <param name="fence">The fence to wait until it's signalled</param>
	/// <param name="fenceValue">The value to query for</param>
	/// <param name="fenceEvent">the event to wait on</param>
	/// <param name="duration">the maximum duration to wait for</param>
	void WaitForFenceValue(ID3D12Fence* fence, uint64_t fenceValue, HANDLE fenceEvent, std::chrono::milliseconds duration = std::chrono::milliseconds::max());
	/// <summary>Wait until all previous commands have finished executing. Ensures back buffer resources have finsihed executing. A Singal followed by WaitForFenceValue</summary>
	/// <param name="commandQueue">The command queue signal</param>
	/// <param name="fence">The fence to wait until it's signalled</param>
	/// <param name="fenceValue">The value to query for</param>
	/// <param name="fenceEvent">the event to wait on</param>
	void Flush(ID3D12CommandQueue* commandQueue, ID3D12Fence* fence, uint64_t fenceValue, HANDLE fenceEvent);

};



