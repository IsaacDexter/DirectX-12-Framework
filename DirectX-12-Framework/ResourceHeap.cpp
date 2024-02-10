#include "ResourceHeap.h"

ResourceHeap::ResourceHeap() :
    m_heapSize(NULL),
    m_freeHandles(),
    m_textures(),
    m_models(),
    m_constantBuffers()
{

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
const std::shared_ptr<Texture> ResourceHeap::CreateTexture(ID3D12Device* device, ID3D12PipelineState* pipelineState, const wchar_t* path, std::string name)
{

    if (!m_load)
    {
        m_commandAllocator->Reset();
        m_commandList->Reset(m_commandAllocator.Get(), pipelineState);
    }

    auto resource = ReserveSRV(name);
    resource->Initialize(device, m_commandList.Get(), path);

    m_load = true;
    return resource;
}

const std::shared_ptr<Texture> ResourceHeap::ReserveSRV(std::string name)
{
    ResourceHandle resourceHandle = GetFreeHandle();
    UINT rootParameterIndex = RootParameterIndices::SRV;

    auto resource = std::make_shared<Texture>(resourceHandle, rootParameterIndex, name);
    m_textures.emplace(name, resource);
    return resource;
}
const std::shared_ptr<ConstantBuffer> ResourceHeap::CreateCBV()
{
    ResourceHandle resourceHandle = GetFreeHandle();
    UINT rootParameterIndex = RootParameterIndices::CBV;

    auto resource = std::make_shared<ConstantBuffer>(resourceHandle, rootParameterIndex, "");
    m_constantBuffers.emplace(resource);
    return resource;
}
const std::shared_ptr<Primitive> ResourceHeap::CreateModel(ID3D12Device* device, ID3D12PipelineState* pipelineState, ID3D12RootSignature* rootSignature, const wchar_t* path, std::string name)
{
    if (!m_load)
    {
        m_commandAllocator->Reset();
        m_commandList->Reset(m_commandAllocator.Get(), pipelineState);
    }

    // Create the model
    auto model = std::make_shared<Primitive>(name);
    model->Initialize(device, m_commandList.Get(), pipelineState, rootSignature, path);
    m_models.emplace(name, model);

    m_load = true;
    return model;

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

const ResourceHandle ResourceHeap::GetFreeHandle()
{

    // If there are no existing freed handles to reuse
    if (m_freeHandles.empty())
    {
        // Store the next 'final' offset, representing the next never allocated offset
        static UINT nextOffset = 0;
        // Create a new set of handles from the next offset 
        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle(m_heap->GetCPUDescriptorHandleForHeapStart(), nextOffset, m_heapSize);
        CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle(m_heap->GetGPUDescriptorHandleForHeapStart(), nextOffset, m_heapSize);
        nextOffset++;
        return ResourceHandle(cpuDescriptorHandle, gpuDescriptorHandle);
    }

    // Or, If there are existing freed handles to reuse
    // Pop the first one and return it
    ResourceHandle handle(m_freeHandles.front());
    m_freeHandles.pop();
    return handle;
}


