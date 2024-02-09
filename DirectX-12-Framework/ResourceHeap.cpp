#include "ResourceHeap.h"

ResourceHeap::ResourceHeap() :
    m_heapSize(NULL)
{
    m_freedOffsets = std::queue<UINT>();
    m_resources = std::unordered_map< std::shared_ptr<Resource>, UINT>();
};

void ResourceHeap::Initialize(ID3D12Device* device, ID3D12PipelineState* pipelineState)
{
    // Describe and create a Shader Resource View (SRV) heap for the texture.
    // This heap also contains the Constant Buffer Views. These are in the same heap because
    // CBVs, SRVs, and UAVs can be combined into a single descriptor table.
    // This means the descriptor heap needn't be changed in the command list, which is slow and rarely used.
    // https://learn.microsoft.com/en-us/windows/win32/direct3d12/resource-binding-flow-of-control

    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1024;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;  // SRV type
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  // Allow this heap to be bound to the pipeline
    ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_heap)));
    m_heap->SetName(L"m_srvCbvHeap");

    // How much to offset the shared SRV/SBV heap by to get the next available handle
    m_heapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)), "Couldn't create command allocator.\n");

    // Create command list, and set it to closed state
    ThrowIfFailed(device->CreateCommandList(
        0,  // 0 for single GPU, for multi-adapter
        D3D12_COMMAND_LIST_TYPE_DIRECT, // Create a direct command list that the GPU can execute
        m_commandAllocator.Get(),   // Command allocator associated with this list
        pipelineState,  // Pipeline state
        IID_PPV_ARGS(&m_commandList)
    ), "Failed to create command list.\n");

    m_load = true;

}
const std::shared_ptr<Texture> ResourceHeap::CreateSRV(ID3D12Device* device, ID3D12PipelineState* pipelineState, const wchar_t* path)
{
    if (!m_load)
    {
        m_commandAllocator->Reset();
        m_commandList->Reset(m_commandAllocator.Get(), pipelineState);
    }

    auto resource = ReserveSRV(device);
    resource->Initialize(device, m_commandList.Get(), path);

    m_load = true;
    return resource;
}
const std::shared_ptr<Texture> ResourceHeap::ReserveSRV(ID3D12Device*)
{
    UINT freeOffset = GetFreeIndex();

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle(m_heap->GetCPUDescriptorHandleForHeapStart(), freeOffset, m_heapSize);
    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle(m_heap->GetGPUDescriptorHandleForHeapStart(), freeOffset, m_heapSize);
    UINT rootParameterIndex = RootParameterIndices::SRV;
    auto resource = std::make_shared<Texture>(cpuDescriptorHandle, gpuDescriptorHandle, rootParameterIndex);
    m_resources.emplace(resource, freeOffset);
    return resource;
}
const std::shared_ptr<ConstantBuffer> ResourceHeap::CreateCBV()
{
    UINT freeOffset = GetFreeIndex();

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle(m_heap->GetCPUDescriptorHandleForHeapStart(), freeOffset, m_heapSize);
    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle(m_heap->GetGPUDescriptorHandleForHeapStart(), freeOffset, m_heapSize);
    UINT rootParameterIndex = RootParameterIndices::CBV;
    auto resource = std::make_shared<ConstantBuffer>(cpuDescriptorHandle, gpuDescriptorHandle, rootParameterIndex);
    m_resources.emplace(resource, freeOffset);
    return resource;
}
bool ResourceHeap::Remove(std::shared_ptr<Resource> resource)
{
    // Check that such a resource is managed by this, and if so, solely by this, before deleting 
    auto it = m_resources.find(resource);
    if (it != m_resources.end())
    {
        if (it->first.use_count() <= 1)
        {
            m_freedOffsets.push(it->second);
            m_resources.erase(it);
            return true;
        }
    }
    return false;
}
bool ResourceHeap::Load(ID3D12CommandQueue* commandQueue)
{
    bool load = m_load;
    if (load)
    {
        m_load = false;
        m_commandList->Close();

        ID3D12CommandList* loadCommandLists[] = { m_commandList.Get() };
        commandQueue->ExecuteCommandLists(_countof(loadCommandLists), loadCommandLists);
    }
    return load;
}

const UINT ResourceHeap::GetFreeIndex()
{
    // If no CBVs have been freed
    if (m_freedOffsets.empty())
    {
        // Get the next available free slot in the array
        return m_resources.size();
    }
    // if CBVs have been freed, use their offsets first
    else
    {
        auto index = m_freedOffsets.front();
        m_freedOffsets.pop();
        return index;
    }
}

