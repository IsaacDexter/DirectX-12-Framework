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
    DescriptorHeap(ID3D12Device* device, ID3D12PipelineState* pipelineState);
    bool Load(ID3D12CommandQueue* commandQueue);
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
    virtual void CreateHeap(ID3D12Device* device) = 0;
    void CreateCommandList(ID3D12Device* device, ID3D12PipelineState* pipelineState);

	// Shader resource view heap for accessing data in a resource (texture)
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
    UINT m_descriptorSize;

    /**
    * Queue of indexes that have been intentionally freed
    * These are always written to over the end of the list as it eliminates the to check if array has been exceeded
    * While a priority queue would encourage contiguous placement of resources, such contiguity cannot be expected, and this makes deletion from the array (i.e. pushing to the queue) cheaper
    */
    std::queue<ResourceHandle> m_freeHandles;

    bool m_load = false;
    bool m_resetRequired = false;

    const ResourceHandle GetFreeHandle();
};

