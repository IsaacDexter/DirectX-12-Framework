#include "SceneObject.h"
#include "Model.h"
#include "Texture.h"
#include "ConstantBuffer.h"

using namespace DirectX;
using namespace Microsoft::WRL;

SceneObject::SceneObject(std::shared_ptr<Model> model, std::shared_ptr<Texture> texture, std::shared_ptr<ConstantBuffer> constantBuffer) :
    m_model(model),
    m_texture(texture),
    m_constantBuffer(constantBuffer),
    m_position(0.0f, 0.0f, 0.0f)
{
}

void SceneObject::Initialize()
{
}

void SceneObject::Update(const float& deltaTime, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection)
{
    // Update constant buffer
    XMMATRIX model = GetModel();
    m_constantBuffer->Update(model, view, projection);
}

void SceneObject::Draw(ID3D12GraphicsCommandList* commandList)
{
    m_texture->Set(commandList);
    m_constantBuffer->Set(commandList);
    m_model->Draw(commandList);
}
