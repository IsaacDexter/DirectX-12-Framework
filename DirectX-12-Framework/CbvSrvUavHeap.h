#pragma once
#include "DescriptorHeap.h"
class CbvSrvUavHeap : public DescriptorHeap
{
public:
    CbvSrvUavHeap(ID3D12Device* device, const D3D12_DESCRIPTOR_HEAP_DESC desc, ID3D12PipelineState* pipelineState);
    bool Load(ID3D12CommandQueue* commandQueue);

    const std::shared_ptr<Texture> CreateTexture(ID3D12Device* device, ID3D12PipelineState* pipelineState, const wchar_t* path, std::string name);
    const std::shared_ptr<Texture> ReserveSRV(std::string name);
    const std::shared_ptr<ConstantBuffer> CreateCBV();
    const std::shared_ptr<Primitive> CreateModel(ID3D12Device* device, ID3D12PipelineState* pipelineState, ID3D12RootSignature* rootSignature, const wchar_t* path, std::string name);

protected:
    friend class Renderer;
    void CreateCommandList(ID3D12Device* device, ID3D12PipelineState* pipelineState);

    // Shader resource view heap for accessing data in a resource (texture)
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;

    bool m_load = false;
    bool m_resetRequired = false;

    /**
    * std::array could be useful for contiguous memory with constant buffer views and shader resource views stored acontiguously in memory
    * std::unordered_map and vector are practically the same with UINT
    * You could template this whole class and have it only handle its own type, creating the DescriptorHeap elsewehere taking a start index for that portion of the heap
    * This would make SRVs and CBVs contiguous, but leave a big gap in descriptor heap. Perhaps this is okay though?
    * You could either seperate the class or have one overarching class. This reduces ownership of a single comptr, which is nice, but also sort of makes less sense.
    * It would make reorganising the heap easier though.
    * I think contigous constant buffers is high priority, having a manager would let you iterate over all of them in one, which could be nice. Just Update the view and proj when needed.
    */
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
    std::unordered_map<std::string, std::shared_ptr<Primitive>> m_models;
    std::unordered_set<std::shared_ptr<ConstantBuffer>> m_constantBuffers;
};

