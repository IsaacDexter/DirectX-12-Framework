#include "SceneObject.h"
#include "Model.h"
#include "Texture.h"
using namespace DirectX;
using namespace Microsoft::WRL;

SceneObject::SceneObject(std::shared_ptr<Model> model, std::shared_ptr<Texture> texture) :
    m_model(model),
    m_texture(texture),
    m_position(0.0f, 0.0f, 0.0f)
{
}

void SceneObject::Initialize(ID3D12Device* device, ID3D12PipelineState* pipelineState, ID3D12RootSignature* rootSignature)
{
    // Create bundle allocator
    ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&m_bundleAllocator)), "Couldn't create command bundle.\n");

    // Create and record the bundle for drawing the triangle
    {
        ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, m_bundleAllocator.Get(), pipelineState, IID_PPV_ARGS(&m_bundle)));
        m_bundle->SetGraphicsRootSignature(rootSignature);
        m_bundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_bundle->IASetVertexBuffers(0, 1, &m_model->GetVertexBufferView());
        m_bundle->IASetIndexBuffer(&m_model->GetIndexBufferView());
        m_bundle->DrawIndexedInstanced(m_model->GetNumIndices(), 1, 0, 0, 0);
        ThrowIfFailed(m_bundle->Close());
    }
}

void SceneObject::Draw(ID3D12GraphicsCommandList* commandList)
{
    m_texture->Set(commandList);
    commandList->ExecuteBundle(m_bundle.Get());
}
