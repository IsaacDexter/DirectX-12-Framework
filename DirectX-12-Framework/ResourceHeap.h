#pragma once
#include "stdafx.h"
#include <unordered_map>
#include <queue>
#include "Resource.h"
#include "ConstantBuffer.h"
#include "Texture.h"

class ResourceHeap
{
public:
    ResourceHeap();
    void Initialize(ID3D12Device* device, ID3D12PipelineState* pipelineState);
    

    const std::shared_ptr<Texture> CreateSRV();
    const std::shared_ptr<ConstantBuffer> CreateCBV();
    bool Remove(std::shared_ptr<Resource> resource);

    ID3D12DescriptorHeap* GetHeap()
    {
        return m_heap.Get();
    }

    enum RootParameterIndices
    {
        SRV,
        CBV,
        Sampler,
    };
protected:

	// Shader resource view heap for accessing data in a resource (texture)
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	// How much to offset the shared SRV/SBV heap by to get the next available handle
	UINT m_heapSize;
    /** 
    * std::array could be useful for contiguous memory with constant buffer views and shader resource views stored acontiguously in memory
    * std::unordered_map and vector are practically the same with UINT 
    * You could template this whole class and have it only handle its own type, creating the DescriptorHeap elsewehere taking a start index for that portion of the heap
    * This would make SRVs and CBVs contiguous, but leave a big gap in descriptor heap. Perhaps this is okay though?
    * You could either seperate the class or have one overarching class. This reduces ownership of a single comptr, which is nice, but also sort of makes less sense.
    * It would make reorganising the heap easier though.
    * I think contigous constant buffers is high priority, having a manager would let you iterate over all of them in one, which could be nice. Just Update the view and proj when needed. 
    */
	std::unordered_map<std::shared_ptr<Resource>, UINT> m_resources;
    /**
    * Queue of indexes that have been intentionally freed
    * These are always written to over the end of the list as it eliminates the to check if array has been exceeded
    * While a priority queue would encourage contiguous placement of resources, such contiguity cannot be expected, and this makes deletion from the array (i.e. pushing to the queue) cheaper
    */
    std::queue<UINT> m_freedOffsets;

    const UINT GetFreeIndex();
};

