#include "RenderTexture.h"

void RenderTexture::Initialize(ID3D12Device* device)
// Create the render texture
{
	// Describe the render texture
	D3D12_RESOURCE_DESC srvDesc = CD3DX12_RESOURCE_DESC::Tex2D(
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

	// Create the SRV in an implicit heap that encompasses it
	{
		// Upload with a default heap
		auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		ThrowIfFailed(device->CreateCommittedResource(
			&uploadHeapProps,
			D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,   // Perhaps D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES? https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_heap_flags
			&srvDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // We want to be able to render this as a texture, we will change to D3D12_RESOURCE_STATE_RENDER_TARGET to update the texture
			&clearValue,
			IID_PPV_ARGS(&resource)    //Store in the renderTexture
		));
		resource->SetName(L"Render Texture SRV");
	}

	// Setup the description of the render target view as the texture.
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = srvDesc.Format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	// TODO : apply the RTV desc
	device->CreateRenderTargetView(resource.Get(), nullptr, rtvCpuDescriptorHandle);
	// Create the shader resource view
	device->CreateShaderResourceView(resource.Get(), nullptr, cpuDescriptorHandle);
}

void RenderTexture::BeginDraw(ID3D12GraphicsCommandList* commandList)
// Indicate that render texture will be used as the render target
{
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrier);

	// Set CPU handles for Render Target Views (RTVs) and Depth Stencil Views (DSVs) heaps
	commandList->OMSetRenderTargets(1, &rtvCpuDescriptorHandle, FALSE, &dsvCpuDescriptorHandle);

	// Record commands.
	// Clear the RTVs and DSVs
	const float clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
	commandList->ClearRenderTargetView(rtvCpuDescriptorHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvCpuDescriptorHandle,  // Aforementioned handle to DSV heap
		D3D12_CLEAR_FLAG_DEPTH, // Clear just the depth, not the stencil
		1.0f,   // Value to clear the depth to  
		0,  // Value to clear the stencil view
		0, nullptr  // Clear the whole view. Set these to only clear specific rects.
	);
}

void RenderTexture::EndDraw(ID3D12GraphicsCommandList* commandList)
{
	// Indicate that the resource will now be used as an SRV, not an RTV
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandList->ResourceBarrier(1, &barrier);
}
