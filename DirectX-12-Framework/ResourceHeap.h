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

class ResourceHeap
{
public:
    ResourceHeap();
    void Initialize(ID3D12Device* device, ID3D12PipelineState* pipelineState);
    

    const std::shared_ptr<Texture> CreateTexture(ID3D12Device* device, ID3D12PipelineState* pipelineState, const wchar_t* path);
    const std::shared_ptr<Texture> ReserveSRV(const wchar_t* path);
    const std::shared_ptr<ConstantBuffer> CreateCBV();
    const std::shared_ptr<Primitive> CreateModel(ID3D12Device* device, ID3D12PipelineState* pipelineState, ID3D12RootSignature* rootSignature, const wchar_t* path);

    ID3D12DescriptorHeap* GetHeap()
    {
        return m_heap.Get();
    }

    bool Load(ID3D12CommandQueue* commandQueue);
    

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
	std::unordered_map<std::wstring, std::shared_ptr<Texture>> m_textures;
	std::unordered_map<std::wstring, std::shared_ptr<Primitive>> m_models;
	std::unordered_set<std::shared_ptr<ConstantBuffer>> m_constantBuffers;
    /**
    * Queue of indexes that have been intentionally freed
    * These are always written to over the end of the list as it eliminates the to check if array has been exceeded
    * While a priority queue would encourage contiguous placement of resources, such contiguity cannot be expected, and this makes deletion from the array (i.e. pushing to the queue) cheaper
    */
    std::queue<ResourceHandle> m_freeHandles;

    bool m_load = false;

    const ResourceHandle GetFreeHandle();
};

