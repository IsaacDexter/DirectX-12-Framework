#include "SceneObject.h"

#include "Resource.h"
#include "Primitive.h"
#include "ConstantBufferView.h"

using namespace DirectX;
using namespace Microsoft::WRL;


SceneObject::SceneObject(std::shared_ptr<Primitive> model, std::shared_ptr<Resource> texture, std::shared_ptr<ConstantBufferView> constantBuffer, std::string name)
    : m_model(model)
    , m_texture(texture)
    , m_constantBuffer(constantBuffer)
    , m_name(name)
    , m_position(0.0f, 0.0f, 0.0f)
    , m_rotation(0.0f, 0.0f, 0.0f)
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_forward(0.0f, 0.0f, -1.0f)	// Used in determining camera direction

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
    XMVECTOR qRotationV = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
    XMStoreFloat4(&m_boundingBox.Orientation, qRotationV);
    // Get the portal's forward, i.e. where its facing 
    auto forward = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
    // Rotate the portal's forward according to the rotation
    auto newForward = XMVector3Rotate(forward, qRotationV);
    newForward = XMVector3Normalize(newForward);
    XMStoreFloat3(&m_forward, newForward);
}

const DirectX::XMMATRIX SceneObject::GetWorld() const
{
    XMMATRIX translation = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
    XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
    XMMATRIX scale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
    return scale * rotation * translation;
}
