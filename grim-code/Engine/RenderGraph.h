#pragma once

#if PLATFORM_PC
#include <Engine/DXEnums.h>
#endif

#include <Engine/CommandContext.h>
#include <Engine/GraphicsState.h>
#include <Engine/Surface.h>
#include <Engine/Buffer.h>

#include <Shared/Core/grimJson.h>

namespace gfx
{
	struct RenderPass;	
	typedef int32_t SurfaceIndex;
	typedef int32_t BufferIndex;

	struct RenderPassModule
	{
		enum
		{
			kModuleNameLength = 64,
		};

		// note: uint64_t here is used as an alias for the pointer type
		typedef void(*RenderPassCallback)(CommandContext*, const RenderPass, size_t);
		typedef size_t(*RenderPassDataCallback)();

		char moduleName[kModuleNameLength];
		RenderPassCallback passCallback;
		RenderPassDataCallback dataCallback;
	};

	struct RenderPass
	{
		enum
		{
			kPassNameLength = 64,
			kMaxBuffers = 8
		};

		GraphicsState state;
		View view;
		SurfaceIndex renderTarget[8];
		SurfaceIndex depthTarget;

		BufferIndex inputs[kMaxBuffers];
		BufferIndex outputs[kMaxBuffers];

		RenderPassModule* module;
		char passName[kPassNameLength];
	};

	enum ResourceType
	{
		kUnknownResourceType,
		kSurface,
		kBuffer
	};

	struct RenderGraphNode
	{
		enum 
		{
			kMaxDependencies = 8
		};

		RenderGraphNode():
			numDependencies(0)
		{}

		RenderPass pass;
		RenderGraphNode* dependencies[kMaxDependencies];
		int numDependencies;

		gfx::Fence fence;
		uint64_t fenceValue;
	};

	class RenderGraph
	{
	public:
		enum
		{
			kFramebufferIndex = -1,
			kMaxSurfaces = 16,
			kMaxBuffers = 32,
			kMaxNodes = 32,
			kMaxModules = 32
		};

		RenderGraph();
		~RenderGraph();

		void Initialize();
		void Shutdown();

		void Compile(const grimJson::JsonValue& renderSettings);
		void Execute(CommandContext* ctx);

		Surface* GetSurface(const SurfaceIndex index);
		Buffer* GetBuffer(const BufferIndex index);

	private:
		void Reset();
		void RunSort();

		SurfaceIndex CreateAndRegisterSurface(SurfaceDefinition& def);
		BufferIndex CreateAndRegisterBuffer(BufferDefinition& def);

		RenderGraphNode* m_nodes;
		Surface* m_surfaces;
		Buffer* m_buffers;

		int m_numNodes;
		int m_numSurfaces;
		int m_numBuffers;

#if _DEBUG
		grimJson::JsonValue m_renderSettings;
		SurfaceDefinition m_surfaceDefinitions[kMaxSurfaces];
		BufferDefinition m_bufferDefinitions[kMaxSurfaces];
#endif
	};

	void InitStaticPassModules();

	RenderPassModule* GetRenderPassModuleFromString(const char* string);
	RenderPass RegisterRenderPass(const GraphicsState& state, const View& view);
	void BeginPass(const RenderPass& passDef, CommandContext* ctx);
	void EndPass(const RenderPass& passDef, CommandContext* ctx);
}

extern gfx::RenderGraph g_renderGraph;