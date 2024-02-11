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

CbvSrvUavHeap::CbvSrvUavHeap(ID3D12Device* device, ID3D12PipelineState* pipelineState) : DescriptorHeap(device, pipelineState)
{
    CreateHeap(device);
}

void CbvSrvUavHeap::CreateHeap(ID3D12Device* device)
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
    ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_descriptorHeap)));
    m_descriptorHeap->SetName(L"m_descriptorHeap");

    // How much to offset the shared SRV/SBV heap by to get the next available handle
    m_descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
