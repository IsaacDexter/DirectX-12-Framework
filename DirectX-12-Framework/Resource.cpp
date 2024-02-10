#include "Resource.h"

Resource::Resource(const ResourceHandle resourceHandle, const UINT& rootParameterIndex) :
    m_resourceHandle(resourceHandle),
    m_rootParameterIndex(rootParameterIndex)
{
}

void Resource::Set(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetGraphicsRootDescriptorTable(m_rootParameterIndex, m_resourceHandle.gpuDescriptorHandle);
}
