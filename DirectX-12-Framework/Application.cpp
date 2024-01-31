#include "Application.h"
#include "Window.h"

using namespace Microsoft::WRL;
using namespace DirectX;

Application::Application(HINSTANCE hInstance)
{
    m_window = std::make_shared<Window>(hInstance);
    m_window->Show();
}

void Application::Initialize()
{
	// Initialize Pipeline
	// Initialize Assets
	InitializePipeline();
	InitializeAssets();

}

void Application::Update()
{
}

void Application::Render()
{
    /*
    - Populate command list
	    - Reset command list allocator
		    - Re-use memory associated with command allocator
	    - Reset command list
	    - Set graphics root signature
		    - To use with current command list
	    - Set viewport and scissor rect
	    - Set *resource barrier*s
		    - Indicate back buffer to be used as render target
	    - Record commands into command list
	    - Indicate back buffer will be used to present after command list execution
		    - Set resource barrier
	    - Close command list
    - Execute command list
    - Present frame
    - Wait for GPU to finish
	    - Wait on fence
    */
    
    // Record all rendering commands into the command list
    PopulateCommandList();

    // Execute the command list
    // Put the command list into an array (of one) for execution on the queue
    ID3D12CommandList* commandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

    // Present the frame
    ThrowIfFailed(m_swapChain->Present(1, 0), "Failed to present frame.\n");

    // Wait for GPU to finish
    WaitForPreviousFrame();
}

void Application::Destroy()
{
    // Wait for the GPU to be done with all resources.
    WaitForPreviousFrame();

    CloseHandle(m_fenceEvent);
}

/*
- Enable debug layer
- Create device
- Create command queue
- Create swap chain
- Create RTV descriptor heap
- Create frame resources
- Create command allocator
*/
void Application::InitializePipeline()
{
#if defined (_DEBUG)
    auto debugLayer = EnableDebugLayer();
#endif
    // create the device
    auto hardwareAdapter = GetAdapter(m_useWarpDevice);
    m_device = CreateDevice(hardwareAdapter);
    //CreateDevice();

    // create the direct command queue
    m_commandQueue = CreateCommandQueue(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT);

    m_swapChain = CreateSwapChain(m_window->GetHWND(), m_commandQueue, m_window->GetClientWidth(), m_window->GetClientHeight(), m_frameCount);

    // create RTV descriptor heap
    m_rtvHeap = CreateDescriptorHeap(m_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_frameCount, m_rtvDescriptorSize);

    m_renderTargets = CreateRenderTargetViews(m_device, m_rtvHeap, m_swapChain, m_rtvDescriptorSize);
}



Microsoft::WRL::ComPtr<ID3D12Debug> Application::EnableDebugLayer()
{
    // Enable the D3D12 debug layer.
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
    }
    return debugController;
}

Microsoft::WRL::ComPtr<IDXGIAdapter4> Application::GetAdapter(bool useWarpDevice)
{

    // a DXGI factory must be created before querying available adaptors
    ComPtr<IDXGIFactory4> dxgiFactory;
    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    //enables errors to be caught during device creation and while querying adapters
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif //DEBUG

    ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)), "Couldn't create factory.\n");

    ComPtr<IDXGIAdapter1> dxgiAdapter1;
    ComPtr<IDXGIAdapter4> dxgiAdapter4;

    if (useWarpDevice)
    {
        // if a warp device is to be used, EnumWarpAdapter will directly create the warp adapter
        ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)), "Couldn't get adapter.\n");
        // cannot static_cast COM, so .As casts com to correct type
        ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4), "Couldn't get adapter.\n");
    }
    else
    {
        // when not using warp, DXGI factory querys hardware adaptors
        SIZE_T maxDedicatedVideoMemory = 0;
        // enummarate available gpu adapters in the system and iterate through
        for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
        {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
            dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

            // check to see if the adapter can create a D3D12 device without actually  creating it. The adapter with the largest dedicated video memory is favored
            // DXGI_ADAPTER_FLAG_SOFTWARE avoids software rasterizer as we're not using WARP
            if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
                SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(), // create a null ddevice if its a compatible DX12 adapter, if it returns S_OK, it is a compatible DX12 header
                    D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
                dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
            {
                maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
                ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4), "Couldn't get adapter.\n");  // cast the adapter and return it
            }
        }
    }

    return dxgiAdapter4;
}

void Application::GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter)
{
    *ppAdapter = nullptr;
    for (UINT adapterIndex = 0; ; ++adapterIndex)
    {
        IDXGIAdapter1* pAdapter = nullptr;
        if (DXGI_ERROR_NOT_FOUND == pFactory->EnumAdapters1(adapterIndex, &pAdapter))
        {
            // No more adapters to enumerate.
            break;
        }

        // Check to see if the adapter supports Direct3D 12, but don't create the
        // actual device yet.
        if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
        {
            *ppAdapter = pAdapter;
            return;
        }
        pAdapter->Release();
    }

}

Microsoft::WRL::ComPtr<ID3D12Device4> Application::CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter)
{
    ComPtr<ID3D12Device4> d3d12Device3;
    ThrowIfFailed(D3D12CreateDevice(
        adapter.Get(),  //pointer to video adapter to use when creating device
        D3D_FEATURE_LEVEL_11_0, //minimum feature level for successful device creation
        IID_PPV_ARGS(&d3d12Device3)
    ), "Couldn't create Device.\n");  //store device in this argument

#if defined(_DEBUG)

    //used to enable break points based on severity of message and filter messages' creation
    ComPtr<ID3D12InfoQueue> pInfoQueue;
    if (SUCCEEDED(d3d12Device3.As(&pInfoQueue)))    //query infoqueue inteface from comptr.as
    {
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);    //sets a message severity level to break on with debugger
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

        // suppress whole categories of messages
        //D3D12_MESSAGE_CATEGORY Categories[] = {};

        // suppress messages based on their severity level
        D3D12_MESSAGE_SEVERITY Severities[] =
        {
            D3D12_MESSAGE_SEVERITY_INFO
        };

        // suppress individual messages by their ID
        D3D12_MESSAGE_ID DenyIds[] = {
            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // warning when render target is cleared using a clear color
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
        };

        //info queue filter is defined and filter is pushed onto info queue
        D3D12_INFO_QUEUE_FILTER NewFilter = {};
        //NewFilter.DenyList.NumCategories = _countof(Categories);
        //NewFilter.DenyList.pCategoryList = Categories;
        NewFilter.DenyList.NumSeverities = _countof(Severities);
        NewFilter.DenyList.pSeverityList = Severities;
        NewFilter.DenyList.NumIDs = _countof(DenyIds);
        NewFilter.DenyList.pIDList = DenyIds;

        ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
    }

#endif // DEBUG

    return d3d12Device3;
}

void Application::CreateDevice()
{
    ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));

    if (m_useWarpDevice)
    {
        ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        ThrowIfFailed(D3D12CreateDevice(
            warpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_device)
        ));
    }
    else
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        GetHardwareAdapter(factory.Get(), &hardwareAdapter);

        ThrowIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_device)
        ));
    }
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> Application::CreateCommandQueue(Microsoft::WRL::ComPtr<ID3D12Device> device, const D3D12_COMMAND_LIST_TYPE type)
{
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = type;

    ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)), "Couldn't ceate command queue.");

    return commandQueue;
}

Microsoft::WRL::ComPtr<IDXGISwapChain4> Application::CreateSwapChain(HWND hWnd, Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, uint32_t width, uint32_t height, uint32_t bufferCount)
{
    ComPtr<IDXGISwapChain4> dxgiSwapChain4;
    ComPtr<IDXGIFactory4> dxgiFactory4;
    UINT createFactoryFlags = 0;

#if defined (_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    // Create the dxgi factory
    ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)), "Couldn't create swap chain.\n");

    // Create descriptor to describe creation of swap chain;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = width;    // resolution width, 0 to obtain width from output window
    swapChainDesc.Height = height;  // resolution height, 0 to obtain width from output window
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // swap chain format, A four-component, 32-bit typeless format that supports 8 bits per channel including alpha.
    swapChainDesc.Stereo = FALSE;   // if swap chain back buffer is stereo, if specified, so must be a flip-model swap chain
    swapChainDesc.SampleDesc = { 1, 0 };    // sample desc that describes multisampling parameters, must be 1, 0 for flip model
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // describes surface usage and cpu access to back buffer. Shader input or render target output
    swapChainDesc.BufferCount = bufferCount;    // number of buffers in swap chain, must be 2 for flip presentation 
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;   // idenifies resize behaviour of back buffer is not equal to output: stretch, none, aspect ratio stretch
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;   // the presentation model and options for handeling buffer after present: sequential, discard, flip sequential and flip discard. Use flip for better performance.
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;  // transparency behaviour: unspecified, premultiplied, straight or ignored.
    swapChainDesc.Flags = CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0; // Allow tearing if tearing is supported (for variable sync)

    // Create the swap chain from the descriptor
    ComPtr<IDXGISwapChain1> swapChain1;
    ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(
        commandQueue.Get(),
        hWnd,
        &swapChainDesc,
        nullptr,    // fullscreen descriptor, set to nullptr for a windowed swap chain
        nullptr,    // pointer to idxgi output to restrict content to, set to nullptr
        &swapChain1
    ), "Couldn't create swap chain.\n");

    // Disable alt+enter fullscreen, so borderless window fullscreen can be used
    ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER), "Couldn't create swap chain.\n");

    ThrowIfFailed(swapChain1.As(&dxgiSwapChain4), "Couldn't create swap chain.\n");

    return dxgiSwapChain4;
}


Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Application::CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device4> device, const D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors, UINT& descriptorSize)
{
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;

    // Create the descriptor heap descriptor used to describe its creation
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = numDescriptors;   // number of descriptors in the heap
    desc.Type = type;   //types of descriptors in the heap: CRV, SRV, UAV; SAMPLER; RTV; DSV
    // additional flags for shader visible, indicating it can be bound on command list for reference by shaders, only for CBV, SRV, UAV; and samplers
    // additional nodemask for multi-adapter nodes

    // Create the descriptor heap from the descriptor
    ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

    descriptorSize = m_device->GetDescriptorHandleIncrementSize(type);

    return descriptorHeap;
}

std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, Application::m_frameCount> Application::CreateRenderTargetViews(Microsoft::WRL::ComPtr<ID3D12Device4> device, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap, Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain, UINT& rtvDescriptorSize)
{
    std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, Application::m_frameCount> renderTargets;

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

    // Create a RTV for each frame.
    for (UINT n = 0; n < m_frameCount; n++)
    {
        ThrowIfFailed(swapChain->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n])));
        device->CreateRenderTargetView(renderTargets[n].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, rtvDescriptorSize);
    }

    return renderTargets;
}


void Application::UpdateBufferResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, ID3D12Resource** pDestinationResource, ID3D12Resource** pIntermediateResource, size_t numElements, size_t elementSize, const void* bufferData, D3D12_RESOURCE_FLAGS flags)
{
    // size of the buffer in bytes
    size_t bufferSize = numElements * elementSize;

    // Create CPU resource in committed memory large enough to store the buffer
    // Create resource and implicit heap large enough to store it, mapping the resource to the heap

    // create heap properties structure that provides properties for the destination resource's heap
    auto destHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    // pointer to resource desc that describes the destinationresource
    auto destResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags);

    ThrowIfFailed(m_device->CreateCommittedResource(
        &destHeapProperties,
        D3D12_HEAP_FLAG_NONE,   // heap option flags
        &destResourceDesc,
        D3D12_RESOURCE_STATE_COPY_DEST, // state of the resource
        nullptr,    // describes default value for a clear colour
        IID_PPV_ARGS(pDestinationResource)  // globally unique identifier for resource interface
    ));

    // create resource used to transfer CPU buffer data into GPU memory
    // create intermediate buffer resource using upload heap
    // so long as bufferData isn't NULL
    if (bufferData)
    {
        // heap properties structure that provides properties for the intermediate resource's heap
        auto interHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        // pointer to resource desc that describes the intermediate resource
        auto interResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
        ThrowIfFailed(m_device->CreateCommittedResource(
            &interHeapProperties,
            D3D12_HEAP_FLAG_NONE,   // heap option flags
            &interResourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ, // state of the resource
            nullptr,    // describes default value for a clear colour
            IID_PPV_ARGS(pIntermediateResource)  // globally unique identifier for resource interface
        ));

        // transfer CPU buffer data to GPU resources
        // describe subresource data to be uploaded to GPU resource
        D3D12_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pData = bufferData; // pointer to memory block containing subresource data
        subresourceData.RowPitch = bufferSize;  // row width in bytes of the subresource data
        subresourceData.SlicePitch = subresourceData.RowPitch;  // depth width in bytes of the subresource data
        // Upload CPU buffer data to GPU resource in a default heap using an intermediate buffer
        UpdateSubresources(
            commandList.Get(),  // pointer for the command list interface to upload with
            *pDestinationResource,  // destination resource
            *pIntermediateResource, // intermediate resource
            0,  // offset in bytes to intermediate resource
            0,  // index of first subresource in the resource, always 0 for buffer resources
            1,  // NumSubresources number of subresources  in the resource to be updated, always 1 for buffer resources
            &subresourceData    // pointer to array of length NumSubresources of pointers to descriptions of subresource data used for the update
        );
    }
}

bool Application::CheckTearingSupport()
{

    bool allowTearing = false;

    // Create a DXGI 1.4 factory interface and query for the 1.5 interface
    // as DXGI 1.5 may not support graphics debugging tools
    ComPtr<IDXGIFactory4> factory4;
    if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
    {
        ComPtr<IDXGIFactory5> factory5;
        if (SUCCEEDED(factory4.As(&factory5)))
        {
            // Query if tearing is supported
            if (FAILED(factory5->CheckFeatureSupport(
                DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                &allowTearing, sizeof(allowTearing))))
            {
                allowTearing = FALSE;
            }
        }
    }

    return allowTearing;
}

void Application::InitializeAssets()
{
    /*
    - Create empty *root signature*
    - Compile shaders
    - Create vertex input layout
    - Create *pipeline state object*
	    - Create description
	    - Create object
    - Create command list
    - Close command list
    - Create and load vertex buffers
    - Create vertex buffer views
    - Create *fence*
    - Create event handle
    - Wait for GPU to finish
	- Wait on fence
    */

    // Create empty root signature
    // Create root signature descriptor 
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    // Use version 1 of root signature layout
    rootSignatureDesc.Init_1_0(
        0, nullptr, // parameters array and number of elements
        0, nullptr, // static samplers and their number
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT    // Use input assembler, i.e. input layout and vertex buffer
    );


    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    // Serialize root signature so it can be created based on description
    ThrowIfFailed(D3D12SerializeVersionedRootSignature(
        &rootSignatureDesc, // Description of root signature
        &signature, // Memory block to acess the serialized root signature (i.e. to create it)
        &error  // Memory block to access serializer error messages
    ), "Couldn't serialize root signature.\n");

    // Create root signature from serialized one using the device
    ThrowIfFailed(m_device->CreateRootSignature(
        0,  // Set to 0 when using single GPU, for Multi-adapter systems
        signature->GetBufferPointer(),  // Pointer to the data of the blob holding the serialized root signature
        signature->GetBufferSize(), // The length of the serialized root signature
        IID_PPV_ARGS(&m_rootSignature) // GUID of the root signature interface
    ), "Failed to create root signature.\n");



    // Load compiled shaders
    // Load vertex shader from precompiled shader files
    ComPtr<ID3DBlob> vertexShaderBlob;
    ThrowIfFailed(D3DReadFileToBlob(L"VertexShader.cso", &vertexShaderBlob), "Failed to load vertex shader.\n");
    // Load pixel shader from precompiled shader files
    ComPtr<ID3DBlob> pixelShaderBlob;
    ThrowIfFailed(D3DReadFileToBlob(L"PixelShader.cso", &pixelShaderBlob), "Failed to load pixel shader.\n");


    // Define vertex input layout, which describes a single element for the Input Assembler
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        {
            "POSITION", // HLSL semnantic name associated with this element
            0,  // sematic index for the element. Only needed when there are multiple elements with the same semantic, i.e. matrices' components
            DXGI_FORMAT_R32G32B32_FLOAT,    // format of the element data
            0,  // identifier for the input slot for using multiple vertex buffers (0-15)
            D3D12_APPEND_ALIGNED_ELEMENT,   // offset in bytes between each elements, defines current element directly after previous one
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // input data class for input slot: per-vertex/per-instance data
            0   //number of instances to draw
        },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // Create the input layout desc, which defines the organization of input elements to the pipeline state desc
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);

    // Describe depth stencil state for pipeline state object, to be default and disabled
    CD3DX12_DEPTH_STENCIL_DESC1 dsDesc(D3D12_DEFAULT);
    dsDesc.DepthEnable = false;
    dsDesc.StencilEnable = false;

    // Describe rasterizer state for pipeline with default values
    CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);

    // Describe blend state with default values
    CD3DX12_BLEND_DESC blendDesc(D3D12_DEFAULT);

    // define render target count and render target formats
    
    CD3DX12_RT_FORMAT_ARRAY rtvFormats;
    rtvFormats.NumRenderTargets = 1;    // define render target count
    std::fill(std::begin(rtvFormats.RTFormats), std::end(rtvFormats.RTFormats), DXGI_FORMAT_UNKNOWN);   // set to unknown format for unused render targets
    rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;   // define render target format for the first (only) render target

    // Default sampler mode without anti aliasing
    DXGI_SAMPLE_DESC sampleDesc = {};
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    // Create Graphics Pipeline State Object, which maintains shaders
    // Describe PSO with Pipeline State Stream, to create the desc with more flexibility
    CD3DX12_PIPELINE_STATE_STREAM pss;
    pss.InputLayout = inputLayoutDesc;  // Input elements' layout
    pss.pRootSignature = m_rootSignature.Get(); // Pointer to root signature
    pss.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());   // Create shader bytecode describing the vertex shader as it appears in memory
    pss.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());    // Create shader bytecode describing the pixel shader as it appears in memory
    pss.RasterizerState = rasterizerDesc;   // describe rasterizer
    pss.BlendState = blendDesc; // Describe blend state
    pss.DepthStencilState = dsDesc;   // Describe depth-stencil state
    pss.SampleMask = UINT_MAX;  // Define sample mask for blend state, max signifies point sampling 
    pss.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // Set the primitive topology to use triangles to draw
    pss.RTVFormats = rtvFormats;    // Render target count & formats
    pss.SampleDesc = sampleDesc;

    // Wrap Pipeline State Stream into a desc
    D3D12_PIPELINE_STATE_STREAM_DESC pssDesc = {};
    pssDesc.SizeInBytes = sizeof(pss);
    pssDesc.pPipelineStateSubobjectStream = &pss;

    // Pass descriptor into pipeline state to create PSO object
    ThrowIfFailed(m_device->CreatePipelineState(&pssDesc, IID_PPV_ARGS(&m_pipelineState)), "Failed to create pipeline state object.\n");



    // Create command list
    // Create command allocator
    ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)), "Couldn't create command allocator.\n");
    // Create command list, and set it to closed state
    ThrowIfFailed(m_device->CreateCommandList(
        0,  // 0 for single GPU, for multi-adapter
        D3D12_COMMAND_LIST_TYPE_DIRECT, // Create a direct command list that the GPU can execute
        m_commandAllocator.Get(),   // Command allocator associated with this list
        m_pipelineState.Get(),  // Pipeline state
        IID_PPV_ARGS(&m_commandList)
    ), "Failed to create command list.\n");
    m_commandList->Close();


    // Create vertex buffer
    {
        // Define the geometry for a triangle.
        auto aspectRatio = m_window->GetAspectRatio();
        Vertex triangleVertices[] =
        {
            { { 0.0f, 0.25f * aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { { 0.25f, -0.25f * aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            { { -0.25f, -0.25f * aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
        };

        const UINT vbSize = sizeof(triangleVertices);

        // create upload heap to transfer vertex buffers. (not recommended)
        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
        // Create vertex buffer description
        auto vbDesc = CD3DX12_RESOURCE_DESC::Buffer(vbSize);
        // Create the vertex buffer as a committed resource and an implicit heap big enough to contain it, and commit the resource to the heap.
        ThrowIfFailed(m_device->CreateCommittedResource(
            &heapProps,  // The properties of the implicit heap
            D3D12_HEAP_FLAG_NONE,
            &vbDesc,    // Descriptor for the resource
            D3D12_RESOURCE_STATE_GENERIC_READ,  // Required heap state for an upload heap
            nullptr,    // Optimized clear value for render target resources
            IID_PPV_ARGS(&m_vertexBuffer)   // GUID of vertex buffer interface
        ), "Failed to create vertex buffer.\n");

        // Copy vertex data into the vertex buffer
        UINT8* pVertexDataBegin;
        CD3DX12_RANGE readRange(0, 0);        // Resource not intended to be read on CPU
        ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)), "Failed to copy vertex data into vertex buffer.\n");
        memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
        m_vertexBuffer->Unmap(0, nullptr);

        // TODO: create a copy command queue and call UpdateBufferResource

        // create vertex buffer view, used to tell input assembler where vertices are stored in GPU memory
        m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress(); // specify D3D12_GPU_VIRTUAL_ADDRESS that identifies buffer address
        m_vertexBufferView.SizeInBytes = sizeof(triangleVertices);    // specify size of the buffer in bytes
        m_vertexBufferView.StrideInBytes = sizeof(Vertex);  // specify size in bytes of each vertex entry in buffer 
    }



    // Create synchronization objects
    {
        ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        m_fenceValue = 1;

        // Create an event handle to use for frame synchronization.
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }

        // Wait for the command list to execute; we are reusing the same command 
        // list in our main loop but for now, we just want to wait for setup to 
        // complete before continuing.
        WaitForPreviousFrame();
    }
}

void Application::WaitForPreviousFrame()

{
    // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
    // This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
    // sample illustrates how to use fences for efficient resource usage and to
    // maximize GPU utilization.

    // Signal and increment the fence value.
    const UINT64 fence = m_fenceValue;
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
    m_fenceValue++;

    // Wait until the previous frame is finished.
    if (m_fence->GetCompletedValue() < fence)
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void Application::PopulateCommandList()
{
    // Command list allocators can only be reset when the associated 
    // command lists have finished execution on the GPU; apps should use 
    // fences to determine GPU execution progress.
    ThrowIfFailed(m_commandAllocator->Reset());

    // However, when ExecuteCommandList() is called on a particular command 
    // list, that command list can then be reset at any time and must be before 
    // re-recording.
    ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

    // Set necessary state.
    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);

    // Indicate that the back buffer will be used as a render target.
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandList->ResourceBarrier(1, &barrier);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // Record commands.
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    m_commandList->DrawInstanced(3, 1, 0, 0);

    // Indicate that the back buffer will now be used to present.
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1, &barrier);

    ThrowIfFailed(m_commandList->Close());
}

