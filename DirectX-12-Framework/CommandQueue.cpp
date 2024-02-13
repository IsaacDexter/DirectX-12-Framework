#include "CommandQueue.h"

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

using namespace Microsoft::WRL;

CommandQueue::CommandQueue(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type)
    : m_fenceValue(0)
    , m_commandListType(type)
    , m_device(device)
{
    // create command queue
    m_commandQueue = CreateCommandQueue(m_device.Get(), m_commandListType);
    // create fence
    m_fence = CreateFence(m_device.Get());
    // create fence event
    m_fenceEvent = CreateEventHandle();
}

CommandQueue::~CommandQueue()
{
    ::CloseHandle(m_fenceEvent);
}

ComPtr<ID3D12GraphicsCommandList> CommandQueue::GetCommandList(ID3D12PipelineState* initialState)
{
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> commandList;

    // obtain an unused command allocator, that is not currently in flight on the command queue
    // if there are any valid items in the queue, and the first (oldest) one has finished...
    if (!m_commandAllocatorQueue.empty() && IsFenceComplete(m_commandAllocatorQueue.front().fenceValue))
    {
        // reuse it and remove it from the front of the queue
        commandAllocator = m_commandAllocatorQueue.front().commandAllocator;
        m_commandAllocatorQueue.pop();

        // reset it, ready for immediate resuse
        ThrowIfFailed(commandAllocator->Reset());
    }
    // if there are no free command allocators, create one
    else
    {
        commandAllocator = CreateCommandAllocator();
    }



    // obtain a command list ready for reuse
    // if there is a command list in the queue...
    if (!m_commandListQueue.empty())
    {
        //...it will be reusable by nature, so pop it and use it
        commandList = m_commandListQueue.front();
        m_commandListQueue.pop();

        // reset it, ready for immediate reuse
        ThrowIfFailed(commandList->Reset(commandAllocator.Get(), initialState));
    }
    // if there are no free command lists, create one
    else
    {
        commandList = CreateCommandList(commandAllocator.Get(), initialState);
    }



    // associate the command allocator with the command list
    // assign a pointer to the command allocator to the private data of the command list
    // this allows the command list to be easily tracked outside of this class. 
    // The allocator can be retrieved from the command list and pushed back to the command allocator queue
    ThrowIfFailed(commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator.Get()));

    return commandList;
}

uint64_t CommandQueue::ExecuteCommandList(ID3D12GraphicsCommandList* commandList)
{
    commandList->Close();

    // retreive assocaited command allocator from command list. This incrememnts reference counter
    ID3D12CommandAllocator* commandAllocator;
    UINT dataSize = sizeof(commandAllocator);
    ThrowIfFailed(commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator));

    // create a temporary array of command lists from the commandList
    ID3D12CommandList* const ppCommandLists[] = { commandList };

    // pass the array to execution
    m_commandQueue->ExecuteCommandLists(1, ppCommandLists);
    // obtain the fence value that indicates this command allocator can be reused
    uint64_t fenceValue = Signal();

    // assign this fence value to the command allocator and return it to the back of the queue for reuse
    m_commandAllocatorQueue.emplace(CommandAllocatorEntry{ fenceValue, commandAllocator });
    // return the (immediately free again) command list back to its queue
    m_commandListQueue.push(commandList);

    // decrement reference counter to command allocator by releasing this temporary pointer
    commandAllocator->Release();

    // return the fence value to wait for this commandlist to be executed
    return fenceValue;
}

uint64_t CommandQueue::Signal()
{
    return Signal(m_commandQueue.Get(), m_fence.Get(), m_fenceValue);
}

bool CommandQueue::IsFenceComplete(uint64_t fenceValue)
{
    return m_fence->GetCompletedValue() > fenceValue;
}

void CommandQueue::WaitForFenceValue(uint64_t fenceValue)
{
    WaitForFenceValue(m_fence.Get(), fenceValue, m_fenceEvent);
}

void CommandQueue::Flush()
{
    Flush(m_commandQueue.Get(), m_fence.Get(), m_fenceValue, m_fenceEvent);
}

ComPtr<ID3D12CommandAllocator> CommandQueue::CreateCommandAllocator()
{
    //char buffer[500];
    //sprintf_s(buffer, "Created command allocator %i .\n", m_commandAllocatorQueue.size());
    //OutputDebugStringA(buffer);
    // create and return an instance of the command allocator
    return CreateCommandAllocator(m_device.Get(), m_commandListType);
}

ComPtr<ID3D12GraphicsCommandList> CommandQueue::CreateCommandList(ID3D12CommandAllocator* allocator, ID3D12PipelineState* pipelineState)
{
    //char buffer[500];
    //sprintf_s(buffer, "Created command list %i .\n", m_commandListQueue.size());
    //OutputDebugStringA(buffer);
    // create and return an instance of a command list using the specified allocator
    return CreateCommandList(m_device.Get(), allocator, pipelineState, m_commandListType);
}

ComPtr<ID3D12CommandQueue> CommandQueue::CreateCommandQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
{
    ComPtr<ID3D12CommandQueue> d3d12CommandQueue;

    // Create the descriptor for the command queue
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = type;   // Type of command queue to create, out of direct, compute and copy
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // Whether to disable GPU timeouts
    desc.NodeMask = 0;  // set to 0 for single GPU operation, for more infor refer to multi-adapter: https://msdn.microsoft.com/en-us/library/dn933253(v=vs.85).aspx

    // Create the command queue using the descriptor
    ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue)));

    return d3d12CommandQueue;
}

ComPtr<ID3D12Fence> CommandQueue::CreateFence(ID3D12Device* device)
{
    ComPtr<ID3D12Fence> fence;

    ThrowIfFailed(device->CreateFence(
        0,  // initial value for the fence, often 0
        D3D12_FENCE_FLAG_NONE,  // fence flags concerning sharing of the fence
        IID_PPV_ARGS(&fence)    // out
    ));

    return fence;
}

ComPtr<ID3D12CommandAllocator> CommandQueue::CreateCommandAllocator(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
{
    ComPtr<ID3D12CommandAllocator> commandAllocator;

    // create the command allocator of the given type
    ThrowIfFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)));
    // can also specify a globally unqiue identifier for the interface

    return commandAllocator;
}

ComPtr<ID3D12GraphicsCommandList> CommandQueue::CreateCommandList(ID3D12Device* device, ID3D12CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState, D3D12_COMMAND_LIST_TYPE type)
{
    ComPtr<ID3D12GraphicsCommandList> commandList;

    // Create the command list
    ThrowIfFailed(device->CreateCommandList(
        0,  //Node mask, for multi-adapter
        type,   //The type of command list to create: DIRECT, that the gpu can execute; BUNDLE, that can only be executed by a direct command list, inherits GPU state; COMPUTE, specifies a command buffer for computing, COPY, specifies a command buffer for COPYING
        commandAllocator, // Command allocator to create the command lists from
        nullptr,    // pointer to pipeline state object that contains the initial pipeline state for the command list. Little overhead for null, with undefined state
        IID_PPV_ARGS(&commandList)  // out
    ));

    // Command lists are created in the recording state. They must be reset in the first render loop, so they must be closed initially
    ThrowIfFailed(commandList->Close());

    return commandList;
}

HANDLE CommandQueue::CreateEventHandle()
{
    HANDLE fenceEvent;

    //Create the OS event
    fenceEvent = ::CreateEvent(
        NULL,   // security attributes, allowing it to be inherited by child processes
        FALSE,  // create a manual or auto-reset event
        FALSE,  // initial state of the object, signalled or not
        NULL    // name of the event object
    );

    //Ensure it was created
    assert(fenceEvent && "Failed to create fence event.");

    return fenceEvent;
}

uint64_t CommandQueue::Signal(ID3D12CommandQueue* commandQueue, ID3D12Fence* fence, uint64_t& fenceValue)
{
    uint64_t& fenceValueForSignal = ++fenceValue;

    ThrowIfFailed(commandQueue->Signal(fence, fenceValueForSignal));

    return fenceValueForSignal;
}

void CommandQueue::WaitForFenceValue(ID3D12Fence* fence, uint64_t fenceValue, HANDLE fenceEvent, std::chrono::milliseconds duration)
{
    // if the the fence has not yet reached the queried value
    if (fence->GetCompletedValue() < fenceValue)
    {
        // register an event object with the fence, which is signalled once the fence has reached the specific value
        ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
        // wait in turn for that event to a complete, to a maximum duration
        ::WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
    }
}

void CommandQueue::Flush(ID3D12CommandQueue* commandQueue, ID3D12Fence* fence, uint64_t fenceValue, HANDLE fenceEvent)
{
    // return the fence value to wait for
    uint64_t fenceValueForSignal = Signal(commandQueue, fence, fenceValue);
    // Wiat on that value, blocking the calling thread, meaning it is safe to release GPU referenced resources
    WaitForFenceValue(fence, fenceValueForSignal, fenceEvent);
}
