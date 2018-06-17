#pragma once

typedef unsigned int uint32_t;

#define kNumDisplayBuffers 2 // todo fix

namespace gfx 
{
	extern uint32_t g_frameCounter;

	class CommandContext;
	struct GraphicsState;
	struct ResourceState;

	enum CommandQueueIndex
	{
		kGfxQueue,
		kCompute0,
		kCompute1,
		kNumCommandQueues
	};

	void Init();
	void ReadAndCompileRenderSettings();
	void SyncForShutdown();
	void Shutdown();

	void SubmitAllContexts();
	void FlipAndAdvanceFrame();

	bool ShouldClose();
	void TriggerClose();

	void SetWindowTitle(const char* text);

	int GetCurrentCommandContextIndex();
	int GetCurrentBackbufferIndex();
	CommandContext* GetCommandContext(int threadID);
}