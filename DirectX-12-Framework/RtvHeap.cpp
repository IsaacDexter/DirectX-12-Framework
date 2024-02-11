#include "RtvHeap.h"

RtvHeap::RtvHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_DESC desc) : 
    DescriptorHeap(device, desc)
{
}

std::shared_ptr<Rtv> RtvHeap::CreateRtv(ID3D12Device* device, ID3D12Resource* resource)
{
    std::shared_ptr<Rtv> rtv;

    ResourceHandle resourceHandle = GetFreeHandle();
    device->CreateRenderTargetView(resource, nullptr, resourceHandle.cpuDescriptorHandle);
    rtv = std::make_shared<Rtv>(resourceHandle, resource);
    m_rtvs.emplace(rtv);
    return rtv;
}

