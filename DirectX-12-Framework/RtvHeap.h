#pragma once
#include "DescriptorHeap.h"

class Rtv
{
private:
    const ResourceHandle m_resourceHandle;
    Microsoft::WRL::ComPtr <ID3D12Resource> m_resource;
public:
    Rtv(const ResourceHandle resourceHandle) : 
        m_resourceHandle(resourceHandle)
    {

    }
    void Reset()
    {
        m_resource.Reset();
    }
    ID3D12Resource* GetResource()
    {
        return m_resource.Get();
    }
    void SetResource(ID3D12Resource* resource)
    {
        m_resource = resource;
    }
    const ResourceHandle GetHandle()
    {
        return m_resourceHandle;
    }
};

class RtvHeap :
    public DescriptorHeap
{
public:
    RtvHeap(ID3D12Device* device, const D3D12_DESCRIPTOR_HEAP_DESC rtvDesc);
    std::shared_ptr<Rtv> CreateRtv();
protected:
    std::unordered_set <std::shared_ptr<Rtv>> m_rtvs;
};

