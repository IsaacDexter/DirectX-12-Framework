#include "CbvSrvUavHeap.h"

const std::shared_ptr<Texture> CbvSrvUavHeap::CreateTexture(ID3D12Device* device, ID3D12PipelineState* pipelineState, const wchar_t* path, std::string name)
{
    if (m_resetRequired)
    {
        m_commandAllocator->Reset();
        m_commandList->Reset(m_commandAllocator.Get(), pipelineState);
        m_resetRequired = false;
    }

    ResourceHandle resourceHandle = GetFreeHandle();
    UINT rootParameterIndex = RootParameterIndices::SRV;

    auto texture = std::make_shared<Texture>(resourceHandle, rootParameterIndex, name);
    // Ensure the load went correctly - if it didnt, return nullptr!
    if (!texture->Initialize(device, m_commandList.Get(), path))
    {
        return nullptr;
    }

    m_textures.emplace(name, texture);


    m_load = true;
    return texture;
}

const std::shared_ptr<Texture> CbvSrvUavHeap::ReserveSRV(std::string name)
{


    ResourceHandle resourceHandle = GetFreeHandle();
    UINT rootParameterIndex = RootParameterIndices::SRV;

    auto resource = std::make_shared<Texture>(resourceHandle, rootParameterIndex, name);
    m_textures.emplace(name, resource);
    return resource;
}

const std::shared_ptr<ConstantBuffer> CbvSrvUavHeap::CreateCBV()
{
    ResourceHandle resourceHandle = GetFreeHandle();
    UINT rootParameterIndex = RootParameterIndices::CBV;

    auto resource = std::make_shared<ConstantBuffer>(resourceHandle, rootParameterIndex, "");
    m_constantBuffers.emplace(resource);
    return resource;
}

const std::shared_ptr<Primitive> CbvSrvUavHeap::CreateModel(ID3D12Device* device, ID3D12PipelineState* pipelineState, ID3D12RootSignature* rootSignature, const wchar_t* path, std::string name)
{
    if (m_resetRequired)
    {
        m_commandAllocator->Reset();
        m_commandList->Reset(m_commandAllocator.Get(), pipelineState);
        m_resetRequired = false;
    }

    // Create the model
    auto model = std::make_shared<Primitive>(name);
    // If the model is loaded incorrectly, return nothing.
    if (!model->Initialize(device, m_commandList.Get(), pipelineState, rootSignature, path))
    {
        return nullptr;
    }
    m_models.emplace(name, model);
    m_load = true;
    return model;

}

CbvSrvUavHeap::CbvSrvUavHeap(ID3D12Device* device, const D3D12_DESCRIPTOR_HEAP_DESC desc, ID3D12PipelineState* pipelineState) :
    DescriptorHeap(device, desc)
{
    CreateCommandList(device, pipelineState);
}

void CbvSrvUavHeap::CreateCommandList(ID3D12Device* device, ID3D12PipelineState* pipelineState)
{
    ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)), "Couldn't create command allocator.\n");

    // Create command list, and set it to closed state
    ThrowIfFailed(device->CreateCommandList(
        0,  // 0 for single GPU, for multi-adapter
        D3D12_COMMAND_LIST_TYPE_DIRECT, // Create a direct command list that the GPU can execute
        m_commandAllocator.Get(),   // Command allocator associated with this list
        pipelineState,  // Pipeline state
        IID_PPV_ARGS(&m_commandList)
    ), "Failed to create command list.\n");
}

bool CbvSrvUavHeap::Load(ID3D12CommandQueue* commandQueue)
{
    bool load = m_load;
    if (load)
    {
        m_load = false;
        m_commandList->Close();

        ID3D12CommandList* loadCommandLists[] = { m_commandList.Get() };
        commandQueue->ExecuteCommandLists(_countof(loadCommandLists), loadCommandLists);
        m_resetRequired = true;
    }
    return load;
}