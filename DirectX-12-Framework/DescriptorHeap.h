#pragma once
#include "stdafx.h"
#include <queue>

class DescriptorHeap
{
public:
    DescriptorHeap(ID3D12Device* device, const D3D12_DESCRIPTOR_HEAP_DESC desc);
    ID3D12DescriptorHeap* GetDescriptorHeap()
    {
        return m_descriptorHeap.Get();
    }

    enum RootParameterIndices
    {
        SRV,
        CBV,
        Sampler,
    };

    void GetFreeHandle(D3D12_CPU_DESCRIPTOR_HANDLE& cpuDescriptorHandle, D3D12_GPU_DESCRIPTOR_HANDLE& gpuDescriptorHandle);
    void GetFreeHandle(D3D12_CPU_DESCRIPTOR_HANDLE& cpuDescriptorHandle);
    void Free(const D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle, const D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle);
    void Free(const D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle);
protected:
    virtual void CreateHeap(ID3D12Device* device, const D3D12_DESCRIPTOR_HEAP_DESC desc);


    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
    UINT m_descriptorSize;

    /**
    * Queue of indexes that have been intentionally freed
    * These are always written to over the end of the list as it eliminates the to check if array has been exceeded
    * While a priority queue would encourage contiguous placement of resources, such contiguity cannot be expected, and this makes deletion from the array (i.e. pushing to the queue) cheaper
    */
    std::queue<std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>> m_freeHandles;

};

