#include "Precompiled.h"

#include <Engine/Gfx.h>

#if PLATFORM_PC
#include <Engine/DXGfx.h>
#endif

#include <Engine/CommandContext.h>
#include <Engine/EngineGlobals.h>
#include <Engine/Fence.h>
#include <Engine/Filesystem.h>
#include <Engine/RenderGraph.h>

namespace gfx {

	static bool s_shouldClose = false;
	uint32_t g_frameCounter = 0;

	const int kNumCommandContexts = 2;
	static CommandContext s_commandContext[kNumCommandContexts];
	static int s_commandIndex = 0;

	void Init()
	{
		plat::InitGfx();

		for (int i = 0; i < kNumCommandContexts; ++i)
		{
			CommandContext& ctx = s_commandContext[i];
			gfx::InitCommandContext(ctx);
		}

		g_renderGraph.Initialize();
		gfx::ReadAndCompileRenderSettings();
	}

	void ReadAndCompileRenderSettings()
	{
		char path[256];
		snprintf(path, 256, "%s\\config.txt", CODE_PATH);
		char* config = grimFile::LoadTextFile(path);

		grimJson::JsonDocument doc;
		grimJson::ParseDocument(doc, config);

		const grimJson::JsonValue& renderSettings = doc["renderSettings"];

		g_renderGraph.Compile(renderSettings);

		delete config;
	}

	void SyncForShutdown()
	{
		for (int i = 0; i < kNumCommandContexts; ++i)
		{
			gfx::DestroyCommandContext(s_commandContext[i]);
		}
	}

	void Shutdown()
	{
		g_renderGraph.Shutdown();
		plat::ShutdownGfx();
	}

	void SubmitAllContexts()
	{
		gfx::CommandContext* ctx = gfx::GetCommandContext(s_commandIndex);

		ctx->Submit();

		ctx->InsertFence(kGfxQueue);
	}

	void FlipAndAdvanceFrame()
	{
		plat::Flip();

		int nextCommandIndex = ++s_commandIndex % kNumCommandContexts;
		s_commandIndex = nextCommandIndex;

		g_frameCounter++;
	}

	bool ShouldClose()
	{
		return s_shouldClose;
	}

	void TriggerClose()
	{
		s_shouldClose = true;
	}

	void SetWindowTitle(const char* text)
	{
		plat::SetWindowTitle(text);
	}

	int GetCurrentCommandContextIndex()
	{
		return s_commandIndex;
	}

	int GetCurrentBackbufferIndex()
	{
		return plat::GetCurrentBackbufferIndex();
	}

	CommandContext* GetCommandContext(int threadID)
	{
		return &s_commandContext[threadID];
	}
}