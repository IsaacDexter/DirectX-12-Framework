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


