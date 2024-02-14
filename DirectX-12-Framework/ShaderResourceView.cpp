#include "ShaderResourceView.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"

bool ShaderResourceView::Load(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, const wchar_t* path)
{
    std::unique_ptr<uint8_t[]> ddsData;
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    HRESULT hr = DirectX::LoadDDSTextureFromFile(device, path, &resource, ddsData, subresources);
    if (hr != S_OK)
    {
        return false;
    }

    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(resource.Get(), 0,
        static_cast<UINT>(subresources.size()));

    // Create the GPU upload buffer.

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

    auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

    ThrowIfFailed(device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadResource)
    ), "Couldn't Create texture.\n");

    UpdateSubresources(commandList, resource.Get(), uploadResource.Get(),
        0, 0, static_cast<UINT>(subresources.size()), subresources.data());

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &barrier);

    DirectX::CreateShaderResourceView(device, resource.Get(), cpuDescriptorHandle);
    return true;
}
