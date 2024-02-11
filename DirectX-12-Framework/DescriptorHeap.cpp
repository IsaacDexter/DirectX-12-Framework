#include "DescriptorHeap.h"

void DescriptorHeap::CreateCommandList(ID3D12Device* device, ID3D12PipelineState* pipelineState)
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


DescriptorHeap::DescriptorHeap(ID3D12Device* device, ID3D12PipelineState* pipelineState) : 
    m_freeHandles()
{
    CreateCommandList(device, pipelineState);
}

bool DescriptorHeap::Load(ID3D12CommandQueue* commandQueue)
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

const ResourceHandle DescriptorHeap::GetFreeHandle()
{

    // If there are no existing freed handles to reuse
    if (m_freeHandles.empty())
    {
        // Store the next 'final' offset, representing the next never allocated offset
        static UINT nextOffset = 0;
        // Create a new set of handles from the next offset 
        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), nextOffset, m_descriptorSize);
        CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), nextOffset, m_descriptorSize);
        nextOffset++;
        return ResourceHandle(cpuDescriptorHandle, gpuDescriptorHandle);
    }

    // Or, If there are existing freed handles to reuse
    // Pop the first one and return it
    ResourceHandle handle(m_freeHandles.front());
    m_freeHandles.pop();
    return handle;

}


