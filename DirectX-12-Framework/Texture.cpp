#include "Texture.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"
// For handling wstring and string dissimilarirties 
#include <codecvt>
using namespace DirectX;

Texture::Texture(const ResourceHandle resourceHandle, const UINT& srvRootParameterIndex, const wchar_t* path) :
    Resource(resourceHandle, srvRootParameterIndex),
    m_path(path)
{
}

void Texture::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<uint8_t[]> ddsData;
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    ThrowIfFailed(
        LoadDDSTextureFromFile(device, m_path.c_str(), &m_resource,
            ddsData, subresources), "Coudln't load texture.\n ");

    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_resource.Get(), 0,
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
        IID_PPV_ARGS(&m_uploadRes)
    ), "Couldn't Create texture.\n");

    UpdateSubresources(commandList, m_resource.Get(), m_uploadRes.Get(),
        0, 0, static_cast<UINT>(subresources.size()), subresources.data());

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_resource.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &barrier);

    CreateShaderResourceView(device, m_resource.Get(), m_resourceHandle.cpuDescriptorHandle);
}

std::string Texture::GetPath()
{
    return std::string(m_path.begin(), m_path.end());
}