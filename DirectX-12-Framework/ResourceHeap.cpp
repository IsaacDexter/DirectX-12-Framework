#include "ResourceHeap.h"

ResourceHeap::ResourceHeap() :
    m_heapSize(NULL),
    m_resources()
{

};

void ResourceHeap::Initialize(ID3D12Device* device)
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

}
const std::shared_ptr<Texture> ResourceHeap::CreateSRV()
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

