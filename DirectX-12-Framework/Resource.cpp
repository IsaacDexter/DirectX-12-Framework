#include "Resource.h"

Resource::Resource(const D3D12_CPU_DESCRIPTOR_HANDLE& cpuDescriptorHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& gpuDescriptorHandle, const UINT& rootParameterIndex):
    m_cpuDescriptorHandle(cpuDescriptorHandle),
    m_gpuDescriptorHandle(gpuDescriptorHandle),
    m_rootParameterIndex(rootParameterIndex)
{
}

void Resource::Set(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetGraphicsRootDescriptorTable(m_rootParameterIndex, m_gpuDescriptorHandle);
}
