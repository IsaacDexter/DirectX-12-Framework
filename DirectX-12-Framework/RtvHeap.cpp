#include "RtvHeap.h"

RtvHeap::RtvHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_DESC desc) : 
    DescriptorHeap(device, desc)
{
}

std::shared_ptr<Rtv> RtvHeap::CreateRtv()
{
    ResourceHandle resourceHandle = GetFreeHandle();

    auto rtv = std::make_shared<Rtv>(resourceHandle);
    m_rtvs.emplace(rtv);
    return rtv;
}

