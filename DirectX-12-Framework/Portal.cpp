#include "Portal.h"
#include "Camera.h"
#include "ShaderResourceView.h"

using namespace DirectX;

Portal::Portal(ID3D12Device* device, const D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuDescriptorHandle, std::shared_ptr<Primitive> model, std::shared_ptr<ShaderResourceView> texture, std::shared_ptr<ConstantBufferView> constantBuffer, std::string name)
	: m_rtvCpuDescriptorHandle(rtvCpuDescriptorHandle)
	, SceneObject(model, texture, constantBuffer, name)
{
	//auto forward = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	//auto rotation = XMQuaternionRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
	XMFLOAT3 direction;
	//XMStoreFloat3(&direction, rotation * forward);
	direction = XMFLOAT3(0.0f, 0.0f, -1.0f);
	m_camera = std::make_unique<Camera>(m_position, direction, DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), m_scale.x / m_scale.y);
	SetPosition(m_position);
	//SetRotation(m_rotation);
	SetScale(m_scale);
	// Create the render texture
	{
		// Describe the render texture
		D3D12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R8G8B8A8_UNORM,  // Use established DS format
			1280,  // Depth Stencil to encompass the whole screen (ensure to resize it alongside the screen.)
			720,
			1,  // Array size of 1
			1,  // MUST NEVER BE 0 OR IT BREAKS
			1, 0,   // Sample count and quality (no Anti-Aliasing)
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET // Allow a DSV to be created for the resource and allow it to handle write/read transitions
		);

		D3D12_CLEAR_VALUE clearValue = { DXGI_FORMAT_R8G8B8A8_UNORM, {} };
		const float clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
		memcpy(clearValue.Color, clearColor, sizeof(clearValue.Color));

		// Create the DSV in an implicit heap that encompasses it
		{
			// Upload with a default heap
			auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			ThrowIfFailed(device->CreateCommittedResource(
				&uploadHeapProps,
				D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,   // Perhaps D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES? https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_heap_flags
				&textureDesc,
				//D3D12_RESOURCE_STATE_RENDER_TARGET, // We we will want to change to this state later
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // We want to be able to render to this texture
				&clearValue,    // Need no depth optimized clear value
				IID_PPV_ARGS(&m_texture->resource)    //Store in the renderTexture
			));
			m_texture->resource->SetName(L"Render Texture SRV");
		}

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		// Setup the description of the render target view.
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		// Create the render target view.
		device->CreateRenderTargetView(m_texture->resource.Get(), nullptr, m_rtvCpuDescriptorHandle);
		// Create the shader resource view
		device->CreateShaderResourceView(m_texture->resource.Get(), nullptr, m_texture->cpuDescriptorHandle);
	}
}

void Portal::DrawTexture(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, std::set<std::shared_ptr<SceneObject>>& objects)
// Indicate that render texture will be used as the render target
{
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_texture->resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrier);

	// Set CPU handles for Render Target Views (RTVs) and Depth Stencil Views (DSVs) heaps
	commandList->OMSetRenderTargets(1, &m_rtvCpuDescriptorHandle, FALSE, &dsvHandle);

	// Record commands.
	// Clear the RTVs and DSVs
	const float clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
	commandList->ClearRenderTargetView(m_rtvCpuDescriptorHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(
		dsvHandle,  // Aforementioned handle to DSV heap
		D3D12_CLEAR_FLAG_DEPTH, // Clear just the depth, not the stencil
		1.0f,   // Value to clear the depth to  
		0,  // Value to clear the stencil view
		0, nullptr  // Clear the whole view. Set these to only clear specific rects.
	);
	// Update Model View Projection (MVP) Matrix according to camera position

	// Draw objects from the view of the other portal
	if (m_otherPortal)
	{
		for (auto object : objects)
		{
			if (object->GetTexture() == m_texture)
			{
				continue;
			}

			object->UpdateConstantBuffer(m_otherPortal->GetView(), m_otherPortal->GetProj());
			//object->UpdateConstantBuffer(m_camera->GetView(), m_camera->GetProj());
			object->Draw(commandList);

		}
	}

	// Indicate that the back buffer will now be used to present.
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_texture->resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandList->ResourceBarrier(1, &barrier);
}

void Portal::SetPosition(const DirectX::XMFLOAT3& position)
{
	SceneObject::SetPosition(position);
	m_camera->SetPosition(position);
}

void Portal::SetRotation(const DirectX::XMFLOAT3& rotation)
{
	SceneObject::SetRotation(rotation);
	// TODO : Update camera direction according to rotation
	// To get the direction the camera should be in:
	// * find the direction from the player camera to this portal?
	// * this will give the illusion of the portal being a seamless part of the world

}

void Portal::SetScale(const DirectX::XMFLOAT3& scale)
{
	SceneObject::SetScale(scale);
	m_camera->SetAspectRatio(scale.x / scale.y);
}


const DirectX::XMMATRIX Portal::GetView()
{
	return m_camera->GetView();
}

const DirectX::XMMATRIX Portal::GetProj()
{
	return m_camera->GetProj();
}
