#include "SceneObject.h"
#include "Primitive.h"
#include "Texture.h"
#include "ConstantBuffer.h"

using namespace DirectX;
using namespace Microsoft::WRL;

SceneObject::SceneObject(std::shared_ptr<Primitive> model, std::shared_ptr<Texture> texture, std::shared_ptr<ConstantBuffer> constantBuffer, std::string name) :
    m_model(model),
    m_texture(texture),
    m_constantBuffer(constantBuffer),
    m_name(name),
    m_aabb(m_position, m_scale),
    m_position(0.0f, 0.0f, 0.0f),
    m_rotation(0.0f, 0.0f, 0.0f),
    m_scale(0.5f, 0.5f, 0.5f)
{

}

void SceneObject::Initialize()
{
}

void SceneObject::Update(const float& deltaTime, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection)
{
    // Update constant buffer
    XMMATRIX model = GetWorld();
    m_constantBuffer->Update(model, view, projection);
}

void SceneObject::Draw(ID3D12GraphicsCommandList* commandList)
{
    if (m_texture)
        m_texture->Set(commandList);
    if (m_constantBuffer)
        m_constantBuffer->Set(commandList);
    if (m_model)
        m_model->Draw(commandList);
}
