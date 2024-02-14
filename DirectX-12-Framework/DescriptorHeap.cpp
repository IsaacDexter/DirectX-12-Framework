#include "DescriptorHeap.h"


DescriptorHeap::DescriptorHeap(ID3D12Device* device, const D3D12_DESCRIPTOR_HEAP_DESC desc) :
    m_freeHandles()
{
    CreateHeap(device, desc);
}

void DescriptorHeap::CreateHeap(ID3D12Device* device, const D3D12_DESCRIPTOR_HEAP_DESC desc)
{
    ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_descriptorHeap)));
    m_descriptorHeap->SetName(L"m_descriptorHeap");

    // How much to offset the shared SRV/SBV heap by to get the next available handle
    m_descriptorSize = device->GetDescriptorHandleIncrementSize(desc.Type);
}

void DescriptorHeap::GetFreeHandle(D3D12_CPU_DESCRIPTOR_HANDLE& cpuDescriptorHandle, D3D12_GPU_DESCRIPTOR_HANDLE& gpuDescriptorHandle)
{

    // If there are no existing freed handles to reuse
    if (m_freeHandles.empty())
    {
        // Store the next 'final' offset, representing the next never allocated offset
        static UINT nextOffset = 0;
        // Create a new set of handles from the next offset 
        cpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), nextOffset, m_descriptorSize);
        gpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), nextOffset, m_descriptorSize);
        nextOffset++;
    }
    else
    {
        // Or, If there are existing freed handles to reuse
        // Pop the first one and return it
        cpuDescriptorHandle = m_freeHandles.front().first;
        gpuDescriptorHandle = m_freeHandles.front().second;
        m_freeHandles.pop();
    }
}

void DescriptorHeap::GetFreeHandle(D3D12_CPU_DESCRIPTOR_HANDLE& cpuDescriptorHandle)
{
    // If there are no existing freed handles to reuse
    if (m_freeHandles.empty())
    {
        // Store the next 'final' offset, representing the next never allocated offset
        static UINT nextOffset = 0;
        // Create a new set of handles from the next offset 
        cpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), nextOffset, m_descriptorSize);
        nextOffset++;
    }
    else
    {
        // Or, If there are existing freed handles to reuse
        // Pop the first one and return it
        cpuDescriptorHandle = m_freeHandles.front().first;
        m_freeHandles.pop();
    }
}


