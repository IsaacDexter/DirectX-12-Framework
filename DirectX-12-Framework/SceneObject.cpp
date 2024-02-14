#include "SceneObject.h"
#include "Primitive.h"
#include "ShaderResourceView.h"
#include "ConstantBufferView.h"

using namespace DirectX;
using namespace Microsoft::WRL;

SceneObject::SceneObject(std::shared_ptr<Primitive> model, std::shared_ptr<ShaderResourceView> texture, std::shared_ptr<ConstantBufferView> constantBuffer, std::string name) :
    m_model(model),
    m_texture(texture),
    m_constantBuffer(constantBuffer),
    m_name(name),
    m_position(0.0f, 0.0f, 0.0f),
    m_rotation(0.0f, 0.0f, 0.0f),
    m_scale(1.0f, 1.0f, 1.0f)
{
    XMFLOAT4 orientation;
    XMStoreFloat4(&orientation, XMQuaternionRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z));
    m_boundingBox = BoundingOrientedBox(m_position, XMFLOAT3(m_scale.x / 2.0f, m_scale.y / 2.0f, m_scale.z / 2.0f), orientation);

}


void SceneObject::UpdateConstantBuffer(const DirectX::XMMATRIX view, const DirectX::XMMATRIX projection)
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

void SceneObject::SetRotation(const DirectX::XMFLOAT3& rotation)
{
    m_rotation = rotation;
    XMFLOAT4 orientation;
    XMStoreFloat4(&orientation, XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));
    m_boundingBox.Orientation = orientation;
}

void SceneObject::SetRotation(const float& roll, const float& pitch, const float& yaw)
{
    m_rotation.x = roll;
    m_rotation.y = pitch;
    m_rotation.z = yaw;
    XMFLOAT4 orientation;
    XMStoreFloat4(&orientation, XMQuaternionRotationRollPitchYaw(roll, pitch, yaw));
    m_boundingBox.Orientation = orientation;
}

const DirectX::XMMATRIX SceneObject::GetWorld() const
{
    XMMATRIX translation = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
    XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
    XMMATRIX scale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
    return scale * rotation * translation;
}
