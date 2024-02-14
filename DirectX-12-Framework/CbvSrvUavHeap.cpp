#include "CbvSrvUavHeap.h"
#include "ShaderResourceView.h"
#include "ConstantBufferView.h"
#include "Primitive.h"


const std::shared_ptr<ShaderResourceView> CbvSrvUavHeap::CreateShaderResourceView(ID3D12Device* device, ID3D12PipelineState* pipelineState, const wchar_t* path, std::string name)
{
    if (m_resetRequired)
    {
        m_commandAllocator->Reset();
        m_commandList->Reset(m_commandAllocator.Get(), pipelineState);
        m_resetRequired = false;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;
    GetFreeHandle(cpuDescriptorHandle, gpuDescriptorHandle);
    UINT rootParameterIndex = RootParameterIndices::SRV;

    auto srv = std::make_shared<ShaderResourceView>(cpuDescriptorHandle, gpuDescriptorHandle, rootParameterIndex);
    srv->name = name;

    // Ensure the load went correctly - if it didnt, return nullptr!
    if (!srv->Load(device, m_commandList.Get(), path))
    {
        return nullptr;
    }
    m_load = true;

    m_textures.insert(srv);
    return srv;
}

const std::shared_ptr<ShaderResourceView> CbvSrvUavHeap::ReserveShaderResourceView(std::string name)
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;
    GetFreeHandle(cpuDescriptorHandle, gpuDescriptorHandle);
    UINT rootParameterIndex = RootParameterIndices::SRV;

    auto srv = std::make_shared<ShaderResourceView>(cpuDescriptorHandle, gpuDescriptorHandle, rootParameterIndex);
    srv->name = name;

    m_textures.insert(srv);
    return srv;
}

const std::shared_ptr<ConstantBufferView> CbvSrvUavHeap::CreateConstantBufferView(ID3D12Device* device)
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;
    GetFreeHandle(cpuDescriptorHandle, gpuDescriptorHandle);
    UINT rootParameterIndex = RootParameterIndices::CBV;

    auto cbv = std::make_shared<ConstantBufferView>(cpuDescriptorHandle, gpuDescriptorHandle, rootParameterIndex);
    
    cbv->Initialize(device);

    m_constantBuffers.insert(cbv);
    return cbv;
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
    m_load = true;

    m_models.insert(model);
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