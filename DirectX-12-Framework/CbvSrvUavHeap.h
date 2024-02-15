#pragma once
#include "DescriptorHeap.h"
#include <unordered_set>

struct ShaderResourceView;
struct ConstantBufferView;
class Primitive;

class CbvSrvUavHeap : public DescriptorHeap
{
public:
    CbvSrvUavHeap(ID3D12Device* device, const D3D12_DESCRIPTOR_HEAP_DESC desc, ID3D12PipelineState* pipelineState);
    bool Load(ID3D12CommandQueue* commandQueue);

    const std::shared_ptr<ShaderResourceView> CreateShaderResourceView(ID3D12Device* device, ID3D12PipelineState* pipelineState, const wchar_t* path, std::string name);
    const std::shared_ptr<ShaderResourceView> ReserveShaderResourceView(std::string name);
    const std::shared_ptr<ConstantBufferView> CreateConstantBufferView(ID3D12Device* device);
    const std::shared_ptr<Primitive> CreateModel(ID3D12Device* device, ID3D12PipelineState* pipelineState, ID3D12RootSignature* rootSignature, const wchar_t* path, std::string name);


protected:
    
    friend class Renderer;
    void CreateCommandList(ID3D12Device* device, ID3D12PipelineState* pipelineState);

    // Shader resource view heap for accessing data in a resource (texture)
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;

    bool m_load = false;
    bool m_resetRequired = false;
};

