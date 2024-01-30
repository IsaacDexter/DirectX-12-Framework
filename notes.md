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