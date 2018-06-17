#include "Precompiled.h"

#include <Engine/RenderGraph.h>

#include <Engine/EngineGlobals.h>
#include <Engine/Fence.h>
#include <Engine/Memory.h>

#include <Shared/Math/Math.h>

#include <unordered_map> // todo: no
#include <string> // todo: no

extern gfx::View s_view;

// todo: move static modules into a different header/cpp
#include <Engine/ForwardLighting.h>

static gfx::RenderPassModule s_forwardLighting;
static gfx::Surface s_framebuffer[kNumDisplayBuffers];

gfx::RenderGraph g_renderGraph;

namespace gfx
{
	RenderGraph::RenderGraph():
		m_numNodes(0),
		m_numSurfaces(0),
		m_numBuffers(0)
	{}

	RenderGraph::~RenderGraph()
	{}

	void RenderGraph::Initialize()
	{
		m_nodes = new RenderGraphNode[kMaxNodes];
		m_surfaces = new Surface[kMaxSurfaces];
		m_buffers = new Buffer[kMaxBuffers];

		InitFramebufferSurfaces(s_framebuffer);
		InitStaticPassModules();
	}

	void RenderGraph::Shutdown()
	{
		delete[] m_nodes;
		delete[] m_surfaces;
		delete[] m_buffers;

		DestroyFramebufferSurfaces(s_framebuffer);
	}

	// todo: no
	static ResourceType GetResourceTypeFromString(const char* string)
	{
		if (strcmp(string, "buffer") == 0)
			return kBuffer;
		if (strcmp(string, "surface") == 0)
			return kSurface;

		return kUnknownResourceType;
	}

	void RenderGraph::Compile(const grimJson::JsonValue& renderSettings)
	{
		grimJson::JsonAllocator allocator;
		m_renderSettings.CopyFrom(renderSettings, allocator);

		std::unordered_map<std::string, SurfaceIndex> surfaceMap;
		std::unordered_map<std::string, BufferIndex> bufferMap;
		std::unordered_map<std::string, RenderGraphNode*> nodeMap;

		// Loop over all resources, allocate them, store their indices in a map for later use
		const grimJson::JsonValue& resourceList = renderSettings["resources"];
		const int numResources = resourceList.Size();
		for (int i = 0; i < numResources; ++i)
		{
			const grimJson::JsonValue& resource = resourceList[i];

			const int kMaxNameLength = max(SurfaceDefinition::kSurfaceNameMaxLength, BufferDefinition::kBufferNameMaxLength);
			char name[kMaxNameLength] = "";

			GRIM_ASSERT(resource.HasMember("name"), "Resource #%d in RenderSettings has no name!", i);
			snprintf(name, kMaxNameLength, "%s", resource["name"].GetString());

			GRIM_ASSERT(resource.HasMember("type"), "Resource %s in RenderSettings has no type!", name);
			ResourceType type = GetResourceTypeFromString(resource["type"].GetString());

			if (type == kSurface)
			{
				SurfaceDefinition desc;
				snprintf(desc.name, kMaxNameLength, "%s", name);
				
				if (resource.HasMember("useFrameRes") && resource["useFrameRes"].GetBool() == true)
				{
					desc.width = g_engineGlobals.frameBufferWidth;
					desc.height = g_engineGlobals.frameBufferHeight;
				}
				else
				{
					GRIM_ASSERT(resource.HasMember("width") && resource.HasMember("height"), "Surface %s in RenderSettings lacks width and height!", name);
					desc.width  = resource["width"].GetInt();
					desc.height = resource["height"].GetInt();
				}

				GRIM_ASSERT(resource.HasMember("format"), "Surface %s in RenderSettings does not have a format!");
				desc.format = GetDataFormatFromString(resource["format"].GetString());

				SurfaceIndex index = CreateAndRegisterSurface(desc);
				surfaceMap[std::string(name)] = index;
			}
			else if (type == kBuffer)
			{
				BufferDefinition desc;
				snprintf(desc.name, kMaxNameLength, "%s", name);

				if (resource.HasMember("sizeCodeDriven") && resource["sizeCodeDriven"].GetBool() == true)
				{
					desc.sizeAlign = mem::SizeAlign(0, 0);
				}
				else
				{
					GRIM_ASSERT(false, "ToDo");
				}

				GRIM_ASSERT(resource.HasMember("format"), "Surface %s in RenderSettings does not have a format!");
				desc.bufferType = GetBufferTypeFromString(resource["format"].GetString());

				BufferIndex index = CreateAndRegisterBuffer(desc);
				bufferMap[std::string(name)] = index;
			}
			else
				GRIM_ASSERT(false, "bad");
		}

		// loop over all passes, create them, hook up their resources
		const grimJson::JsonValue& passList = renderSettings["passes"];
		const int numPasses = passList.Size();
		for (int i = 0; i < numPasses; ++i)
		{
			const grimJson::JsonValue& pass = passList[i];
			RenderPass renderPass;
			memset(&renderPass, 0, sizeof(RenderPass));

			GRIM_ASSERT(pass.HasMember("name"), "RenderPass #%d in RenderSettings has no name!", i);
			snprintf(renderPass.passName, RenderPass::kPassNameLength, "%s", pass["name"].GetString());

			GRIM_ASSERT(pass.HasMember("module"), "RenderPass %s in RenderSettings has no module!", renderPass.passName);
			renderPass.module = GetRenderPassModuleFromString(pass["module"].GetString());

			// render targets
			if (pass.HasMember("targets") && pass["targets"].Size() > 0)
			{
				const grimJson::JsonValue& targetList = pass["targets"];

				const int numTargets = targetList.Size();
				for (int j = 0; j < numTargets; ++j)
				{
					const grimJson::JsonValue& target = targetList[j];
					const char* targetName = target.GetString();

					if (strcmp(targetName, "framebuffer") == 0)
					{
						renderPass.renderTarget[j] = (SurfaceIndex)kFramebufferIndex;
						continue;
					}

					GRIM_ASSERT(surfaceMap.find(targetName) != surfaceMap.end(), "Can't find target name %s in the RenderSettings surface list", targetName)
					renderPass.renderTarget[j] = surfaceMap[targetName];
				}
			}

			if (pass.HasMember("depthTarget"))
			{
				const grimJson::JsonValue& depthTarget = pass["depthTarget"];
				const char* depthTargetName = depthTarget.GetString();

				GRIM_ASSERT(surfaceMap.find(depthTargetName) != surfaceMap.end(), "Can't find target name %s in the RenderSettings surface list", depthTargetName);
				renderPass.depthTarget = surfaceMap[depthTargetName];
			}

			// buffers
			if (pass.HasMember("inputs") && pass["inputs"].Size() > 0)
			{
				const grimJson::JsonValue& inputList = pass["inputs"];

				const int numInputs = inputList.Size();
				for (int j = 0; j < numInputs; ++j)
				{
					const grimJson::JsonValue& input = inputList[j];
					const char* inputName = input.GetString();

					GRIM_ASSERT(bufferMap.find(inputName) != bufferMap.end(), "Can't find input buffer name %s in the RenderSettings buffer list", inputName);
					renderPass.inputs[j] = bufferMap[inputName];
				}
			}

			if (pass.HasMember("outputs") && pass["outputs"].Size() > 0)
			{
				const grimJson::JsonValue& outputList = pass["outputs"];

				const int numOutputs = outputList.Size();
				for (int j = 0; j < numOutputs; ++j)
				{
					const grimJson::JsonValue& output = outputList[j];
					const char* outputName = output.GetString();

					GRIM_ASSERT(surfaceMap.find(outputName) != surfaceMap.end(), "Can't find output buffer name %s in the RenderSettings buffer list", outputName);
					renderPass.outputs[j] = surfaceMap[outputName];
				}
			}

			// view	todo
			renderPass.view = s_view;

			renderPass.state = GraphicsState::GetDefault();

			// graphics state todo BlendState MultisampleState etc
			if (pass.HasMember("depthState"))
			{
				const grimJson::JsonValue& depthStateDefinition = pass["depthState"];
				DepthStencilState& depthState = renderPass.state.depthStencilState;

				depthState.testEnabled = depthStateDefinition["testEnabled"].GetBool();
				depthState.writeEnabled = depthStateDefinition["writeEnabled"].GetBool();
				depthState.depthFunc = GetDepthFuncFromString(depthStateDefinition["depthFunc"].GetString());
			}

			RenderGraphNode& node = m_nodes[m_numNodes++];
			InitFence(node.fence);
			node.fenceValue = 0;
			node.pass = renderPass;
			//node.dependencies filled out below

			nodeMap[renderPass.passName] = &node;
		}

		for (int i = 0; i < m_numNodes; ++i)
		{
			RenderGraphNode& node = m_nodes[i];
			const grimJson::JsonValue& pass = passList[i]; // guaranteed to be in the same order still
			
			if (pass.HasMember("dependencies") && pass["dependencies"].Size() > 0)
			{
				const grimJson::JsonValue& dependencyList = pass["dependencies"];

				const int numDependencies = dependencyList.Size();
				for (int j = 0; j < numDependencies; ++j)
				{
					const grimJson::JsonValue& dependency = dependencyList[j];

					const char* dependencyName = dependency.GetString();

					GRIM_ASSERT(nodeMap.find(dependencyName) != nodeMap.end(), "Can't find pass node named %s in the RenderGraph", dependencyName);
					node.dependencies[j] = nodeMap[dependencyName];
				}
			}
		}

		RunSort();
	}

	void RenderGraph::Execute(CommandContext* ctx)
	{
		for (int i = 0; i < m_numNodes; ++i)
		{
			const RenderGraphNode& node = m_nodes[i];

			// wait for dependencies
			for (int j = 0; j < node.numDependencies; ++j)
			{
				RenderGraphNode* dependency = node.dependencies[j];
				ctx->GPUWaitForFence(dependency->fence, dependency->fenceValue);
			}

			const RenderPass& pass = node.pass;

			// begin pass
			BeginPass(pass, ctx);

			// build pass data
			size_t passData = pass.module->dataCallback();

			// run callback
			pass.module->passCallback(ctx, pass, passData);

			// end pass
			EndPass(pass, ctx);
		}
	}

	Surface* RenderGraph::GetSurface(const SurfaceIndex index)
	{
		if (index == kFramebufferIndex)
			return &s_framebuffer[gfx::GetCurrentBackbufferIndex()];

		return &m_surfaces[(int)index];
	}

	Buffer* RenderGraph::GetBuffer(const BufferIndex index)
	{
		return &m_buffers[(int)index];
	}

	void RenderGraph::Reset()
	{
		for (int i = 0; i < m_numSurfaces; ++i)
		{
			DestroySurface(m_surfaces[i]);
		}

		for (int i = 0; i < m_numBuffers; ++i)
		{
			DestroyBuffer(m_buffers[i]);
		}

		for (int i = 0; i < m_numNodes; ++i)
		{
			DestroyFence(m_nodes[i].fence);
			m_nodes[i].fenceValue = 0;
		}

		m_numNodes = 0;
		m_numBuffers = 0;
		m_numSurfaces = 0;
	}

	// Kahn's algorithm for topological sorting
	void RenderGraph::RunSort()
	{
		RenderGraphNode* sortedList[kMaxNodes] = {};
		int numInSortedList = 0;

		int edgeCounts[kMaxNodes] = {};

		RenderGraphNode* openList[kMaxNodes] = {};
		int numInOpenList = 0;
		int openListStartIndex = 0;

		// Create a list of all nodes with no dependencies
		for (int i = 0; i < m_numNodes; ++i)
		{
			RenderGraphNode* node = &m_nodes[i];

			if (node->numDependencies == 0)
				openList[numInOpenList++] = node;

			edgeCounts[i] = node->numDependencies;
		}

		while (numInOpenList > 0)
		{
			// Remove node from the open list
			RenderGraphNode* node = openList[openListStartIndex++];
			--numInOpenList;

			// Add node to the sorted list
			sortedList[numInSortedList++] = node;

			// loop through all nodes, skipping this one (todo)
			for (int i = 0; i < m_numNodes; ++i)
			{
				RenderGraphNode* secondNode = &m_nodes[i];
				if (secondNode == node)
					continue;

				// if the second node depends on the first node, decrement edge count
				for (int j = 0; j < secondNode->numDependencies; ++j)
				{
					if (secondNode->dependencies[j] == node)
						edgeCounts[i]--;
					
					// if the second node no longer has edges, add it to the open list
					if (edgeCounts[i] == 0)
						openList[openListStartIndex + numInOpenList] = secondNode;
				}
			}
		}

#if _DEBUG
		// if any edges remain in the graph, then it is not acyclic
		for (int i = 0; i < m_numNodes; ++i)
		{
			GRIM_ASSERT(edgeCounts[i] == 0, "Cycle found for RenderGraph node %s", m_nodes[i].pass.passName);
		}
#endif

		// Copy the list back
		for (int i = 0; i < m_numNodes; ++i)
		{
			m_nodes[i] = *sortedList[i];
		}
	}

	SurfaceIndex RenderGraph::CreateAndRegisterSurface(SurfaceDefinition& def)
	{
		GRIM_ASSERT(m_numSurfaces < kMaxSurfaces, "[RenderGraph] Max number of surfaces hit. Consider increasing this number (currently %d)", kMaxSurfaces);
		CreateSurface(def, m_surfaces[m_numSurfaces]);

#if _DEBUG
		m_surfaceDefinitions[m_numSurfaces] = def;
#endif

		return (SurfaceIndex)m_numSurfaces++;
	}

	BufferIndex RenderGraph::CreateAndRegisterBuffer(BufferDefinition& def)
	{
		GRIM_ASSERT(m_numBuffers < kMaxBuffers, "[RenderGraph] Max number of buffers hit. Consider increasing this number (currently %d)", kMaxBuffers);
		CreateBuffer(def, m_buffers[m_numSurfaces]);

#if _DEBUG
		m_bufferDefinitions[m_numBuffers] = def;
#endif

		return (BufferIndex)m_numBuffers++;
	}

	void InitStaticPassModules()
	{
		// Forward Lighting
		snprintf(s_forwardLighting.moduleName, RenderPassModule::kModuleNameLength, "%s", "forwardLightGeo");
		s_forwardLighting.passCallback = &gfx::ForwardLighting_Execute;
		s_forwardLighting.dataCallback = &gfx::ForwardLighting_GetData;
	}

	// todo: no
	RenderPassModule* GetRenderPassModuleFromString(const char* string)
	{
		RenderPassModule* ret = nullptr;

		if (strcmp(string, "forwardLightGeo") == 0)
			ret = &s_forwardLighting;

		GRIM_ASSERT(ret != nullptr, "Failed to find a module for identifier %s");

		return ret;
	}

	gfx::RenderPass gfx::RegisterRenderPass(const GraphicsState& state, const View& view)
	{
		gfx::RenderPass ret;

		return ret;
	}

	void gfx::BeginPass(const gfx::RenderPass& passDef, CommandContext* ctx)
	{
		ctx->SetGraphicsState(passDef.state);

		const View& view = s_view;
		ctx->SetViewport(view.viewPort);
		ctx->SetScissorRect(); // TEMP

		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		ctx->SetRenderTarget(0, g_renderGraph.GetSurface(passDef.renderTarget[0]));
		ctx->SetDepthTarget(g_renderGraph.GetSurface(passDef.depthTarget));

		ctx->ClearRenderTarget(0, clearColor);
		ctx->ClearDepthTarget(1.0f);
	}

	void gfx::EndPass(const gfx::RenderPass& passDef, CommandContext* ctx)
	{
		// todo 
	}
}