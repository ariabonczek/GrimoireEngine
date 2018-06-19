#include "Precompiled.h"

#include <string.h>
#include <stdio.h>

#include <Engine/CommandContext.h>
#include <Engine/EngineGlobals.h>
#include <Engine/Filesystem.h>
#include <Engine/Gfx.h>
#include <Engine/GfxDebug.h>
#include <Engine/GrimInput.h>
#include <Engine/Memory.h>
#include <Engine/View.h>

#include <Shared/Core/grimJson.h>
#include <Shared/Core/Timer.h>

#include <Tools/GrimoireBuild/Grimoire.h>

// TEMP AHH
#include <Engine/d3dx12.h>
#include <Engine/DXGfx.h>
#include <Engine/Surface.h>
#include <Engine/GraphicsState.h>
#include <Engine/DrawElement.h>
#include <Engine/Buffer.h>
#include <Engine/ForwardLighting.h>
#include <Engine/RenderGraph.h>

static ID3D12PipelineState* m_pipelineState;
static gfx::Shader shader;
static gfx::Buffer vertexBuffer;
static gfx::Buffer indexBuffer;
static gfx::Buffer s_viewBuffer;
static gfx::Buffer s_lightBuffer;
static gfx::GraphicsState m_graphicsState;
static gfx::ResourceState m_resourceState;
static grim::Grimoire* s_grimoire;
gfx::View s_view;
static gfx::RenderView s_renderView;
gfx::DrawElement* s_drawElements;
static gfx::RenderPass s_passDef;

struct LightBuffer
{
	gd::tLight lights[10];
	uint32_t numLights;
};
static LightBuffer s_allLights;

#include <d3dcompiler.h>

// END TEMP

static bool HasArg(const char* buffer, const char* arg)
{
	return strstr(buffer, arg) != nullptr;
}

static void CheckForArgs(char* args)
{
	if (HasArg(args, "-coolSkateboard"))
		g_engineGlobals.hasCoolSkateboard = true;
	if (HasArg(args, "-bootingFromEditor"))
		g_engineGlobals.bootingFromEditor = true;
}

static void HandleArgs(const int argc, char** argv)
{
	char buffer[1024];
	buffer[0] = '\0';
	
	if (argc > 1)
	{
		for (int i = 1; i < argc; ++i)
		{
			strcat(buffer, " ");
			strcat(buffer, argv[i]);
		}
	}
	else
	{
		char path[256];
		snprintf(path, 256, "%s\\argv.txt", CODE_PATH);
		grimFile::LoadTextFile(path, buffer, 1024);
	}

	printf("Starting game with command line arguments:%s\n", buffer);
	CheckForArgs(buffer);
}

static void LoadEngineConfig()
{
	char path[256];
	snprintf(path, 256, "%s\\config.txt", CODE_PATH);
	char* config = grimFile::LoadTextFile(path);

	grimJson::JsonDocument doc;
	grimJson::ParseDocument(doc, config);

	g_engineGlobals.windowWidth			= doc["windowWidth"].GetUint();
	g_engineGlobals.windowHeight		= doc["windowHeight"].GetUint();

	g_engineGlobals.frameBufferWidth	= doc["displayBufferResolutionX"].GetUint();
	g_engineGlobals.frameBufferHeight	= doc["displayBufferResolutionY"].GetUint();
	g_engineGlobals.aspectRatio			= (float)g_engineGlobals.frameBufferWidth / g_engineGlobals.frameBufferHeight;

	g_engineGlobals.numDisplayBuffers	= doc["numDisplayBuffers"].GetUint();

	if(g_engineGlobals.hasCoolSkateboard)
		strcpy(g_engineGlobals.windowTitle, "How bout that skateboard though?");
	else
		strcpy(g_engineGlobals.windowTitle, doc["windowTitle"].GetString());

	delete config;
}

// <<<<<<< TEMP
struct Vertex
{
	float position[3];
	float normals[3];
	float uv[2];
};

// >>>>>>>> TEMP

static void Init()
{
	Clock::Initialize();
	mem::Init();
	grimFile::Init();

	// Loads engine settings from config.txt
	LoadEngineConfig();

	gfx::Init();
	grimInput::InitAndSetupMap(g_engineGlobals.windowWidth, g_engineGlobals.windowHeight);

	// Load the scene file, create all of the resources, initialize state of systems

	// <<<<<<<<< TEMP

	HRESULT hr;

	shader.shaderStages = gfx::kVertex | gfx::kPixel; // TODO: Reflect shader stage information
	gfx::LoadShader(shader, "shaders.hlsl");

	// Create the vertex buffer.
	{
		// pos, normal, uv
		Vertex triangleVertices[] =
		{
			{ { -0.5f,  0.5f, -0.5f }, { -0.577f,  0.577f, -0.577f }, { 0.0f, 0.0f } },
			{ {  0.5f,  0.5f, -0.5f }, {  0.577f,  0.577f, -0.577f }, { 1.0f, 0.0f } },	//	   4--------5
			{ { -0.5f, -0.5f, -0.5f }, { -0.577f, -0.577f, -0.577f }, { 0.0f, 1.0f } },	//	  /|       /|
			{ {  0.5f, -0.5f, -0.5f }, {  0.577f, -0.577f, -0.577f }, { 1.0f, 1.0f } },	//	 / |      / |
																						//	0--------1  |
																						//	|  6-----|--7
			{ { -0.5f,  0.5f,  0.5f }, { -0.577f,  0.577f,  0.577f }, { 0.0f, 1.0f } },	//	| /      | /
			{ {  0.5f,  0.5f,  0.5f }, {  0.577f,  0.577f,  0.577f }, { 0.0f, 0.0f } },	//	|/       |/
			{ { -0.5f, -0.5f,  0.5f }, { -0.577f, -0.577f,  0.577f }, { 1.0f, 1.0f } },	//	2--------3
			{ {  0.5f, -0.5f,  0.5f }, {  0.577f, -0.577f,  0.577f }, { 0.0f, 1.0f } }
		};

		const uint32_t vertexBufferSize = sizeof(triangleVertices);

		gfx::BufferDefinition vertexBufferDefinition = {"Cube Vertex Buffer", triangleVertices, { vertexBufferSize, 0 }, gfx::kVertexBuffer, sizeof(Vertex) };
		gfx::CreateBuffer(vertexBufferDefinition, vertexBuffer);
	}

	// index buffer

	uint16_t indices[] = {
		0, 1, 2, 1, 3, 2,
		1, 5, 3, 5, 7, 3,
		5, 4, 7, 4, 6, 7,
		4, 0, 6, 0, 2, 6,
		4, 5, 0, 5, 1, 0,
		2, 3, 6, 3, 7, 6
	};
	gfx::BufferDefinition indexBufferDefinition = { "Cube Index Buffer", indices,{ sizeof(indices), 0 }, gfx::kIndexBuffer, sizeof(uint16_t) };
	gfx::CreateBuffer(indexBufferDefinition, indexBuffer);

	// view constant buffer
	Matrix viewLW = Matrix::CreateLookAt(Vector3(0.0f, 0.0f, -3.0f), Vector3::kZero, Vector3::kUp);
	s_view = gfx::MakeView(0.25f * 3.141592f, (float)g_engineGlobals.frameBufferWidth, (float)g_engineGlobals.frameBufferHeight, 0.01f, 100.0f, viewLW);

	s_renderView = gfx::RenderViewFromView(s_view);

	gfx::BufferDefinition viewBufferDef{ "View Buffer", &s_renderView, { sizeof(gfx::RenderView), 0 }, gfx::kConstantBuffer, sizeof(gfx::RenderView) };
	gfx::CreateBuffer(viewBufferDef, s_viewBuffer);

	// lightBuffer
	memset(&s_allLights, 0, sizeof(LightBuffer));
	s_allLights.numLights = 1;

	gd::tLight light;
	light.color.r = 1.0f;
	light.color.g = 0.0f;
	light.color.b = 0.0f;
	light.color.a = 1.0f;
	light.direction = Vector3(0.0f, -1.0f, 0.0f);
	light.intensity = 1.0f;
	light.type = gd::tLight::eLightType::kDirectional;
	light.size = 1.0f;

	gfx::BufferDefinition lightBufferDef{ "Light Buffer", &s_allLights,{ sizeof(LightBuffer), 0 }, gfx::kConstantBuffer, sizeof(LightBuffer) };
	gfx::CreateBuffer(lightBufferDef, s_lightBuffer);

	gfx::RasterizerState rs = { gfx::kSolid, gfx::kBack, false };
	gfx::BlendState bs = { false, gfx::kOne, gfx::kZero, gfx::kAdd, gfx::kOne, gfx::kZero, gfx::kAdd };
	gfx::MultisampleState ms = { 1, 0 };
	gfx::DepthStencilState dss = { false, false, gfx::kLessEqual };

	m_graphicsState = { rs, dss, ms,{ bs, bs, bs, bs, bs, bs, bs, bs }, gfx::kTriangleList };
	m_resourceState = { &shader, &vertexBuffer, &indexBuffer, &s_viewBuffer };

	const gfx::Surface* renderTarget[8] = { g_renderGraph.GetSurface(gfx::RenderGraph::kFramebufferIndex) };
	const gfx::Surface* depthRenderTarget = g_renderGraph.GetSurface(0); // fix me

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = plat::StateToPipelineDesc(m_graphicsState, m_resourceState, renderTarget, depthRenderTarget);
	hr = plat::GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&m_pipelineState));
	GRIM_ASSERT(!hr, "Failed to create graphics pipeline state");

	s_drawElements = new gfx::DrawElement[1];
	s_drawElements[0].resourceState = m_resourceState;
	s_drawElements[0].indexCount = 36;

	s_passDef.renderTarget[0] = -1;
	s_passDef.depthTarget = 0;
	s_passDef.view = s_view;
	s_passDef.state = m_graphicsState;

	// >>>>>>>>>>>> END TEMP
}

static void GameLoop()
{
	debug::DebugCameraMovement(s_view, Clock::GetFrameTime());
}

static void RunRenderGraph()
{
	s_renderView = RenderViewFromView(s_view);
	gfx::UploadBufferData(s_viewBuffer);

	const int contextIndex = gfx::GetCurrentCommandContextIndex();
	gfx::CommandContext* ctx = gfx::GetCommandContext(contextIndex);

	gfx::ForwardLighting_Data data;
	data.drawElements = s_drawElements;
	data.numElements = 1;

	ctx->Reset();

	ctx->HACK_SetPipelineState(m_pipelineState);
	//ForwardLighting_Execute(ctx, s_passDef, (size_t)&data);
	g_renderGraph.Execute(ctx);

	ctx->Close();
}

static void Shutdown()
{
	gfx::SyncForShutdown();

	// TEMP
	delete[] s_drawElements;
	m_pipelineState->Release();
	DestroyShader(shader);

	DestroyBuffer(vertexBuffer);
	DestroyBuffer(indexBuffer);
	DestroyBuffer(s_viewBuffer);
	DestroyBuffer(s_lightBuffer);
	// TEMP

	gfx::Shutdown();
	mem::Shutdown();
}

static void Debug_UpdateWindowTitle()
{
	float mouseX = grimInput::GetFloat(grimInput::kMouseReticleX);
	float mouseY = grimInput::GetFloat(grimInput::kMouseReticleY);

	char newTitle[EngineGlobals::kMaxWindowTitleLength];
	float timeSeconds = Clock::GetFrameTime();

	Vector4 eyePosition = s_renderView.eyePosition;

	snprintf(newTitle, EngineGlobals::kMaxWindowTitleLength, "Grimoire Engine - Process Time: %.3f, Frame Time: %.1fms (%.2f fps) Mouse X: %.3f, Mouse Y: %.3f, Cam X: %.3f, Cam Y: %.3f, Cam Z: %.3f", 
		Clock::GetTimeSinceProcessStart(),
		timeSeconds * 1000.0f, 
		1.0f / timeSeconds, 
		mouseX, 
		mouseY,
		eyePosition.x,
		eyePosition.y,
		eyePosition.z	
	);
	EngineGlobals::SetWindowTitle(newTitle);
}

int EngineMain(int argc, char** argv)
{
	HandleArgs(argc, argv);

	Init();

	while (true)
	{
		Clock::FrameTick();

		plat::ProcessGfx();

		if (gfx::ShouldClose())
			break;

		grimInput::Update();
		
		GameLoop();
		RunRenderGraph();

		gfx::SubmitAllContexts();
		gfx::FlipAndAdvanceFrame();

		Debug_UpdateWindowTitle();
	}

	Shutdown();

	return 0;
}