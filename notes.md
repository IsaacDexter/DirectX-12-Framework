# Code Flow
1. Initialize
2. Repeat
	- Update
	- Render
3. Destroy

## Initialize

### Initialize Pipeline
- Enable debug layer
- Create device
- Create command queue
- Create swap chain
- Create RTV *descriptor heap*
- Create frame resources
- Create *command allocator*

#### Descriptor Heap
- Array of descriptors
- Each fully describes an object to the GPU

#### Command Allocator
- Manages underlying storage for *command lists and bundles*

### Initialize Assets
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

#### Root Signature
- Defines what resources are bound to the graphics pipeline

#### Pipeline State Object
- Maintains state of currently set shaders
- Maintains certain fixed function shaders

#### Fence
- Synchronize CPU with GPU

## Update
- Update everything that should change per frame
- Modify constant, vertex, index buffers

## Render
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

### Resource Barrier
- Manage resource transitions

## Destroy
- Wait for GPU to finish
	- Wait on fence
- Close event handle

___

# Work Submission
- In DX11, work submission is handled by the immediate context, which handles multithreading poorly
- In DX12, work submission is based on command lists
	- Contain all the information for executing a workload on the GPU
	- Self contained, pre-computation can be done free-threaded by the driver
	- Constructed, Submitted, Discarded
- Bundles are resused
- Can be played back with different resources, i.e. to draw the same model with different textures
- Lower cost than two command list approach
- Command lists can be reset immediately, command allocators must be waited on before they can be reset.
- Command allocators can grow, not shrink
	- pools should be considered 
	- multiple command lists can record to one allocator before reset, so long as only one list records to an allocator at any given time
	- command lists are easy to pool, as they can be immediately reset and added back to the pool after each execution
- Command lists do not hold references to passed in objects, so one must be careful the ComPtrs aren't destroyed
- Command Queues exist in three types: direct copy and compute
- They can be handed a list of closed, not reset command lists to execute

## Synchronization

### Queues
- Synchronize work between queues
- Integer that represents unit of work currently being processed
- Integer is updated when Signal is called
- This value can be checked to determine if a unit of work has been completed
	- i.e. another operation can be started
	- 

### Resource Barriers
- Synchronize resources accessed by queues
- Declare before & after state of a resource
- Transition, Aliasing and UAV barriers
- barrier transitions should be batched into one call where possible
- Must be set to common/present state on the back buffer before presentation

## Pipeline State
- When geometry is submitted to GPU for drawing settings determine how the input data is interpreted and rendered
- Describe the states of each stage, which shaders are being used
- Any number can be initialized at program start
- Pipeline state can be set with `SetPipelineState` in a command list/bundle
- Certain settings must be set on a graphics command list instead of a state
	- Resource bindings
	- Viewports
	- Scissor Rects
	- Blend Factor
	- Depth Stencil Ref
	- Primitive topology
- Bundles inherit Pipeline State not set with PSOs on creation
	- Must set primitive topology

![image](https://learn.microsoft.com/en-us/windows/win32/direct3d12/images/compute-pipeline.png)

## [Render pass](https://learn.microsoft.com/en-us/windows/win32/direct3d12/direct3d-12-render-passes)
- Subset of recorded commands on command list
- `ID3D12GraphicsCommandList4::BeginRenderPass`
- Set of output bindings to RTVs/DSVs
- GPU operations that target said output bindings
- Metadata that describes dependencies for output bindings

___

# Resource Binding
1. Create root signature objects
1. Create shaders and pipeline state with associated RSOs
1. Create descriptor heaps to contain SRV, UAV, CBV descriptors for each frame
1. Initialize descriptor heaps with descriptors that are reused across many frames
1. For each rendered frame:
	1. For each command list
		1.  Set current root signature to use
			1. Update root signature constants and descriptors (i.e. world/view)
			1. For each item to draw
				1. Define new descriptors as needed for per-object rendering
				1. Update root signature with pointer to needed regions of descriptor heaps
				1. Update root signature constants/descriptors
				1. Set pipeline state for item to draw if changed
				1. Draw
	1. Release memory that is finished being used
	

## Shader resources
- Resources
	- Textures
	- Constant tables
	- Images
	- Buffers
	- SRV
	- CBV
	- UAV
	- Sampler

- Not bound to shader pipeline.
- Instead through a descriptor.

### Descriptor
- Small object containing resource info
- Grouped into descriptor tables

### Descriptor Table
- Group of descriptors
- Resources are accessed by index to descriptor table
- Stored in descriptor heaps

### Descriptor Heap
- Contains all the descriptors in tables for frames to be rendered
- All resources stored in user mode heaps


## Root Signature
- Binding convention
- Shaders use to locate resources
- Stores:
	- Indexes to descriptor tables in a descriptor heap
	- Constants
	- A small number of descriptors directly
		- Such as CBV that changes per draw
		- Saves application from putting descriptors in a heap



# [DX12 Graphics Pipeline](https://www.3dgep.com/learning-directx-12-1/)

## Input assembler stage

* read primitive data from **vertex** & **index** buffers

* assemble data into primitives (triangles)

* *fixed function*

## Vertex shader stage

* transforms vertex data from **object-space** into **clip-space**

* perform skeletal animation

* compute per-vertex lighting

* takes single vertex as input and outputs the clip-space position of that vertex

## Hull shader (optional)

* determines how much an input control patch should be tesselated by the tesselation stage

## Tessellator

* subdivides primitive into smaller primitives, according to tessellation factiors set in hull shader stage

* *fixed function*

## Domain shader (optional)

* computes final vertex attributes based on output control points from hull shader and interpolation coordinates from tessellator stage

* input is a single output from the tesselator, outputs computed attributes of tessellated primitive

## Geometry shader (optional)

* takes a single geometric primitive as input, can discard, change the type of, or generate additional primitives

## Stream output (optional)

* feed primitive data to GPU memory

* can then be recirculated back to the pipeline to be processed by another set of shaders

* *fixed function*

## Rasterizer

* clips primitives into view frustum

* perform front/back-face culling

* interpolate per-vertex attributes accross faces of each primitive

* passes these interpolated balues into pixel shader

* *fixed-function*

## Pixel shader

* takes interpolated per-vertex values from rasterizer

* produces per-pixel color values

* can output a depth value of the current pixel to SV_Depth

* invoked once for each pixel covered by a primitive

## Output merger

* combines output data together with the render targets' currently bound contents

* produces final pipeline result



# GPU synchronization

* in previous versions, this was handled by driver

* in DX12, must be explicitly synchronized

* resources cannot be freed if currently being referenced in a command list being executed on a comm and queue

## Fence

* object used to synchronize commands issued to the **command queue**

* stores a single variable that indicates the last value used to **signal** the fence

* create at least one fence for each command queue

	* multiple command queues can wait on a fence to reach a specific value

	* fence should only be signaled from a single command queue

* application tracks a **fence value** used to signal the fence

## Command list

* issues commands:

	* copy

	* compute (dispatch)

	* draw

* commands are not executed immediately like DX11's immediate context

* commands are **deferred**, only ran on GPU after executed on a command queue

## Command queue

* `ID3D12CommandQueue::ExecuteCommandLists`

* `ID3D12CommandQueue::Signal`

```

method IsFenceComplete( _fenceValue )

    return fence->GetCompletedValue() >= _fenceValue

end method



method WaitForFenceValue( _fenceValue )

    if ( !IsFenceComplete( _fenceValue )

        fence->SetEventOnCompletion( _fenceValue, fenceEvent )

        WaitForEvent( fenceEvent )

    end if

end method



method Signal

    _fenceValue <- AtomicIncrement( fenceValue )

    commandQueue->Signal( fence, _fenceValue )

    return _fenceValue

end method



method Render( frameID )

    _commandList <- PopulateCommandList( frameID )

    commandQueue->ExecuteCommandList( _commandList )

    _nextFrameID <- Present()

    fenceValues[frameID] = Signal()

    WaitForFenceValue( fenceValues[_nextFrameID] )

    frameID <- _nextFrameID

end method

```

* **IsFenceComplete**

    * check to see if fence's completed value has been reached

* **WaitForFenceValue**

    * stall CPU thread until fence value has been reached

* **Signal**

    * append a fence value into the command queue

    * when command queue reaches the value, the fence that appended it will have its completed value set

    * call does not block calling thread, returns value to wait for before writable GPU resources can be reused

* **Render**

    * render a frame

        * poulate command list with all draw/compute commands needed to render the scene

        * execute this list with ExecuteCommandList, which won't block the calling thread

        * doesn't wait for execution on GPU before returning to caller

    * when that frame's previous fence value is reached, move to next frame

    * `Present` will present the rendered result to the screen

        * returns index of next back-buffer within swap-chain to render to

        * `DXGI_SWAP_EFFECT_FLIP_DISCARD` flip model will prevent present from blocking the main thread

        * this means the back-buffer from the previous frame cannot be reused until the image has been presented

        * to prevent this from being overwritten before presentation, CPU thread must wait for the fence value of the previous frame to be reached

            * `WaitForFenceValue` block's CPU thread until the specified fence value has been reached

### Command queue types 

* **Copy**

    * used to issue commands to copy resources data between/on CPU & GPU

* **Compute**

    * can do everything a copy queue can do and issue compute/dispatch commands

* **Direct**

    * can do eveything copy and compute queues can do, and issue draw commands

* allocate one fence object and track one fence value for each allocated command queue

* ensure every command queue tracks its own fence object and fence value, and only signals its own fence object

* a fence value should never be allowed to decrease - the max unsigned int would take 19.5 million years before overflow

# DirectX 12 Device

* used to create resources

* not directly used for isuing draw or dispatch commands

* memory context that tracks allocation in GPU memory



# V-sync

* variable refresh rate displays require tearing to be enabled

* applications must check for vsync off, meaning they must support tearing

* this must be sepcified when creating the swap chain with `DXGI_*_ALLOW_TEARING`



# Swap chain

* presents rendered image to screen

* stores 2+ buffers used to render the screen

    * **back buffer** currently being rendered to

    * **front buffer** currently being presented

* **flip** presentation model

    * passes front buffer to **Desktop Window Manager** (DWM) for presentation to the screen

    * sequential flip

        * keep contents of back buffer after present

        * cannot be used with multisampling

        * may cause presentation lag when there are no more back buffers to use

    * discard flip

        * empty the back buffer after present

        * cannot be used with multisampling, partial presentation

        * achieves maximum framerates with v-sync off



# Descriptor heap

* array of **resource views**

    * **render target views**

    * **shader resource views**

    * **unordered access views**

    * **constant buffer views**

* descriptor heap must be created for resource views

* RTV and **sampler** views require seperate descriptor heaps

* stores RTVs for the sawp chain buffers



# Render target views

* describes a resource to be attatched to a bind slot of the OM stage

* describes the resource that recieves the final colour computed by the pixel shader stage

* needs clearing

* is bound to a buffer on the **swap chain**



# Command allocator

* backing memory used by **command list**

* provides no functionality, accessed indirectly through CL

* used by a single command at a time, but reused after commands have finished executing on GPU

* memory used must be reset after finishing execution (checked with GPU fence)



# Command list

* records commands executed on GPU

* deferred, invoking draw or dispathc omannds are not executed until the list is sent to the **command queue**

* can be reused immediately after execution

* must be reset before recording any new commands

    * DIRECT, that the gpu can execute

    * BUNDLE, that can only be executed by a direct command list, inherits GPU state

    * COMPUTE, specifies a command buffer for computing

    * COPY, specifies a command buffer for COPYING



# [Resource barrier](https://www.3dgep.com/learning-directx-12-1/#Create_the_Render_Target_Views:~:text=another%20using%20a-,resource%20barrier,-and%20inserting%20that)

* ensure resources are in correct state before use

* transitions resource state when inserted into the command list

    * **transition**

        * transitions a resource to a particular state before using it

        * e.g. transition texture to pixel_shader_resource state

    * aliasing

        * specifies a resource is used in a placed/reserve heap

    * UAV

        * all UAV access to the resource have completed before future UAV access can begin

        * necessary when transitioned for read-write, write-read, write-write



___



# Resource usage

* only one interface used to describe resources, regardless of resource type, `ID3D12Resource`

* can control how resource is stored in GPU memory with `ID3D12Heap` interface



# Pipeline state object (PSO)

* describes various stages of rendering pipeline

* combines parts of

    * **input assembly**

    * **vertex shader**

    * **hull shader**

    * **domain shader**

    * **geometry shader**

    * **stream output**

    * **rasterizer stage**

    * **pixel shader**

    * **output merger**

* configuration of pipeline controlled by PSO

    * except **viewport** and **scissor rectangle**

* contains most of state required to configure rendering pipeline

* includes

    * shader bytecode (shaders)

    * vertex format input layout

    * primitive topology type (point, line, triangle, patch)

    * blend state

    * rasterizer state

    * depth-stencil state

    * depth-stencil format

    * render target count and render target format

    * multisample desc

    * stream output buffer desc

    * **root signature**

* excludes

    * vertex/index buffers

    * stream output buffer

    * render targets

    * descriptor heaps

    * shader params

        * constant buffers 

        * read-write buffers

        * read-write textures

    * viewports

    * scissor rects

    * constant blend factor

    * stencil reference value

    * primitive topology and adjacency information

* can be specified for a graphics command list when the command list is reset `ID3D12GraphicsCommandList::Reset`

    * changed for the command list at any time ` ID3D12GraphicsCommandList::SetPipelineState`

* if state needs to change between draw calls, a new PSO needs to be created



# Heaps and resources

* graphics programmer must decide how buffer resources are created

* allocated within a **memory heap**



## Commited resources

* `ID3D12Device::CreateCommittedResource`

* creates resource and implicit heap large enough to hold it

* maps resource to the heap

* easy to manage as one needs not concern over their placement in a heap

* ideal for allocating:

    * large resources like textures

    * statically sized resources

    * large resource in upload heap used for uploading dynamic **vertex/index buffers**

        * useful for UI rendering

        * useful for uploading **constant buffer** data that is changing each draw call



## Placed resources

* explicitly placed in a heap at a specific offset

* heap must be created first `ID3D12Device::CreateHeap`

* create placed resource inside the heap `ID3D12Device::CreatePlacedResource`

* flags can limit which resource can be allocated in a heap

* placed resources can be aliased in a heap if they dont access the same aliased heap space at the same time

    * help reduce oversubscribing GPU memory usage

    * limits size of heap to largest resource at any one moment

    * swapped using **resource aliasing barrier**



pros:

* more performant

    * heap does not need allocation from global GPU memory for each resource

* more options for implementing memory management techniques



cons:

* complex, reqires discipline

* heap size needs to be known in advance

    * bad idea to make excessively large heaps

    * difficult to reclaim memory used by a heap



## Reserved resource

* created without specific heap to place in

* create: `ID3D12Device::CreateReservedResource`

* must be mapped before use, `ID3D12CommandQueue::UpdateTileMappings`

* can be created larger than can fit in a single heap

* portions can be mapped using heaps residing in GPU memory

    * a large volume texture can be created with virtual memory

    * only resident spaces need mapping to physical memory

* provides options for implementing rendering techniques using **sparse voxel octrees**



# Root signature

* describes parameters passed into shader stages in pipeline

    * **constant buffers**

    * **textures**

    * **buffers**

* **texture samplers**

* similar to C++ function signature

    * defines parameters passed into shader pipeline

* **root arguments**

    * values ound to pipeline

* arguments passed to shader can change without changing root signature **parameters**

* also define **shader registers** and **register spaces** to bind the arguments to in the shader



constraints:

* limited to 64 DWORDs

    * **32-bit constants** each cost 1 DWORD, 0 cost of access (in terms of levels of indirection)

    * **inline descriptors** each cost 2 DWORDs, 1 cost of access

    * **descriptor tables** each cost 1 DWORD, 2 cost of access

    * **static samplers** are free

* should be as small as possible, but balanced and flexible

* order parameters based on likelihood of change

* 



## Shader registers/Register spaces

* **shader parameters** must be bound to a register (N = max number of **shader registers**)

    * **constant buffers** bound to *b* registers *b0 - bN*

    * **SRVs** (textures and non-constant buffer types) bound to *t* registers *t0 - tN*

    * **UAVs** (writeabe textures and buffer types) bound to *u* registers *u0 - uN*

    * **texture samplers** bound to *s* registers *s0 - sN*

* all **shader parameters** must be defined for all stages of pipeline

    * registers used across different shader stages may not overlap

    * **register spaces** work around this

        * resources may overlap register slots, so long as they dont overlap register spaces

        * relevant when porting legacy shaders to DX12



## Root signature parameters

* can contain any number of parameters

    * CBV desc

    * SRV desc

    * UAV desc

    * **descriptor table**

    * **32-bit root constant**

    

### 32-bit root constant

* used to pass contant buffer data to shader without making a constant buffer resource

* dynamic indexing into constant buffer is not supported for constant data stored in root signature space

* e.g. matrices can be stored, but arrays cannot as they have indices

    * instead require **inline descriptor** or a descriptor heap to map it

* 1 DWORD cost



### Inline descriptors

* descriptors placed directly in root signature without requiring a descriptor heap

* only these can be accessed using inline descriptors in root signature:

    * **constant buffers** (CBV)

    * buffer resources (SRV, UAV) containing 32-bit float, UINT or SINT components

* inline UAV descritors for buffer resources cannot contain counters

* textures cannot be referenced with inline descriptors

    * must be placed in a descriptor heap and referenced through a **descriptor table**

* constant buffers containing arrays *can* be accessed using inline descriptors in root signature

* 2 DWORD cost



### Descriptor tables

* defines several descriptor ranges placed consecutively in a descriptor heap

* ![A descriptor table (A) in a root signature defines a set of one or more descriptor ranges (B). The descriptor ranges define a set of consecutive descriptors in a GPU visible descriptor heap (C). The descriptors are views to the GPU resources stored in one or more resource heaps (D).](https://www.3dgep.com/wp-content/uploads/2018/01/Descriptor-Tables.png)

* 1 DWORD cost



## Static samplers

* texture sampler describes how a texture is to be sampled

    * filtering

    * addressing

    * LOD

* can be defined directly in root signature

    * `D3D12_STATIC_SAMPLER_DESC`

* require no space and do not count against root signature size limit



___



# Mapping



* **bump mapping** 

	* old techinque

	* greyscale height maps



* **normal mapping**

	* colour normal maps

	* detail through texture lighting calculations



* **displacement mapping**

	* creates additional geometry



## Space



* **tangent space** 

	* normals point up relative to surface

	* also known as texture space



*  **object space**

	* normals are relevant to object



## Rendering mapping techinques



1. map sampling

	* assign texel from normap map to pixel on object/surface

2. per-pixel calculation of detail

	* per-pixel shading

		* pixel lighting using value fetched from map

		* provides surface normal at that point

		* provides illusion of bumps using normal from map

		* finer detail than those captured by mesh geometry



### Application code

1. load mesh model and normal map

1. compute tangents



### Shader code



#### Vertex shader



3. compute position

1. compute light and eye vectors at vertex in world space

1. convert light and eye vectors to tanget space using **Tangent Bitangent and Normal (TBN) matrix**

1. passthrough these to pixel shader with texture coordinate



#### Rasteriser



7. interpolate vertex and texel position, vectors and eye



##### Pixel shader



8. retrieve normal from map, decompress to -1, 1 if needs be

1. compute diffuse and specular reflection at pixel

	> colour; normal; light and eye vectors

	* set reflected light to 0 if source of incident light is below surface

		* allows for geometric self-shadowing

		* no diffuse and specular reflection if 

			* light has negative z

			* angle between bump normal and light vector is > 90



___



# Post Processing



# Render to texture

* half-life 2 included this in one of their technical demos

* render target view

    * container for the texture to render to 

    * as if it has a texture

    * doesn�t necessarily have to output to the monitor

* when you load in a texture, you create a shader resource view





## Creating a custom render texture

* create a texture

* set up a render target description

* create a render target view from that texture

* create a shader resource view with the same texture for the pixel shader to use

* set the render target to this texture to write to it

* use the shader resource view to read from it





# Post-processing

* modifying a screen after it has been rendered

    * but not yet presented to the screen

* effects

    * anti-aliasing

    * colour transformation

    * bloom

    * blur

    * deferred rendering

* less cost than you�d expect, doesn�t need to do geometry, only pixels

## Rendering to texture

1. setup

    1. normal screen render target, A

    1. alternative render target B

    1. create a render texture with render target B

    1. create a quad the size of the screen

        * setup the quad as you would any 3d object

        * create vertex/pixel buffers

        * store the vertices in projection space

        * set the vertex/pixel shader

        * render the quad, sampling the texture in PS
2. render

    1. attach B to device context (replacing A)

    1. render scene but don�t present

    1. scene is now rendered to the render texture

3. render, using the render texture

    1. attach A to device context

    1. use render texture, drawing fullscreen quad with texture

    1. render scene, present





# Anti-aliasing

* super-sampling anti-aliasing SSAA

* multi-sample anti-aliasing MSAA

* fast approximate anti-aliasing FXAA

* coverage sampling antialiasing CSAA

* temporal antialiasing TXAA



## Aliasing

* by drawing a mathematically perfect line onto a pixelated resolution

* this can leave jagged edges

* this is aliasing





# SSAA

1. sample pixels within a region

1. average colour of those pixels

1. redraw region with average colours

* render to a high resolution texture

* expensive

* down sample to screen size, then render that

* old technique, out of fashion



# MSAA

* easily enabled in DirectX

* based on theory of Geometric Aliasing

* occurs only at the edges of triangles

* mipmaps help reduce incidence of pixel aliasing within textures

* high resolution  textures close up, low res far away

* geometry aliasing should be focus

* a sampler is rendered at each pixel, if it is covered by the triangle that pixel is shaders

* MSAA considers these sample points 

* 2-16 samples per pixel

* fidelity/cost





# FXAA

* geometry rasterization causes aliasing

* edges rather than inside triangles

* MSAA requires large colour, depth, stencil buffers

* detect edges, use depth buffer to determine whether edges are geometric

* edge pixels with a similar depth are likely of the same surface, and are discarded

* mark pixels with a degree of being an edge

* filtering takes said degree into account

* advantages

* smooths transparent pixels

* fast

* disadvantages

* can smooth textures

* can�t be applied automatically, needs to be coded





# CSAA

* coverage is the detection of how much a polygon is covering a pixel

* weights sampling based on which polygons are covering pixels

* less samples required

* hardware driven, enabled in the application

* 16x MSAA quality at the speed of 4x MSAA



# TXAA

* only effective when in motion

* samples in and around the pixel and samples from prior frames

* hardware MSAA over time



# Gaussian Blur

* common technique

* take average of a number of pixels, weighted on central pixels

* render the scene to a small texture

* performs some blur for us

* perform horizontal blur

* re-render the scene to another texture

* perform a vertical blur

* upsample back to screen size

* use the texture





## Method

* for each pixel, calculate average colour based on self and each neighbour

* weight the blur based on distance from pixel

* upsample the texture to screen size





## Two-pass

* more efficient

* perform a horizontal blur

* perform a vertical blur

* 9 pixels horizontally then 9 vertically = 18 texture lookups

* 9x9 pixels = 81 texture lookups



# Box / Coffin Blur

* very simple and fast blur

* take the average of a group of pixels



# Bloom / HDR

* light spilling past occluding surfaces

* high-definition range

    * in photography, several images are taken to achieve a better balance between light and dark

* in games, the opposite is true

* purposely simulates the lack of HDR

* ![HDR](https://www.realtimerendering.com/blog/wp-content/uploads/2010/04/mainsf.jpg)



## Method

* re-render a normal image to a lower resolution

* only draw pixels which have a texture alpha set

* if the alpha is 0, black is rendered

* otherwise, the colour * alpha



## Tone Mapping

* consider multiple lights lighting a scene

* RGB is limited to a maximum brightness of 1.0

* allow lighting to exceed the value of 1

    * Change render to texture buffer

    * DXGI_FORMAT_R16G16B16A16_FLOAT

* then convert HDR values to LDR, to bring them back into the range of 0-1

* apply a gaussian blur to the rendered image

* when accessing the pixels in the pixel shader:

    * only blur pixels above a brightness threshold

    * otherwise set to black

* can be done with 8-bit colour component, but use a lower brightness threshold



# Depth of Field Blur

* render scene to a texture (A)

* render again to a texture (B)

    * apply gaussian blur

* render again to another texture (C)

    * write the depth values

    * write the world z values, not depth buffer values

        * depth buffer isn�t linear, while world is

* consider rendering to multiple targets at once to reduce overhead

* draw a fullscreen quad

* include the 3 textures in pixel shader

    * normal

    * blurred

    * depth-value

* decide on a focal point/period to switch between blurred/not blurred

* sample all 3 textures

* use depth value to choose between/blend them using a balance value based on focal point/period

    * `smoothstep`

