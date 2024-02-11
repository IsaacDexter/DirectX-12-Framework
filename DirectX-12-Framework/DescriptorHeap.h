#pragma once
#include "stdafx.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <string>
#include "Resource.h"
#include "ConstantBuffer.h"
#include "Texture.h"
#include "Primitive.h"

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
protected:
    virtual void CreateHeap(ID3D12Device* device, const D3D12_DESCRIPTOR_HEAP_DESC desc);


    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
    UINT m_descriptorSize;

    /**
    * Queue of indexes that have been intentionally freed
    * These are always written to over the end of the list as it eliminates the to check if array has been exceeded
    * While a priority queue would encourage contiguous placement of resources, such contiguity cannot be expected, and this makes deletion from the array (i.e. pushing to the queue) cheaper
    */
    std::queue<ResourceHandle> m_freeHandles;

    const ResourceHandle GetFreeHandle();
};

