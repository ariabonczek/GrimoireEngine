#pragma once

typedef unsigned int uint32_t;

struct EngineGlobals
{
	static void SetWindowTitle(const char* title);

	enum
	{
		kMaxWindowTitleLength = 1024
	};

	void*			windowHandle;
	uint32_t		windowWidth;
	uint32_t		windowHeight;
	
	uint32_t		frameBufferWidth;
	uint32_t		frameBufferHeight;
	float			aspectRatio;

	char			windowTitle[kMaxWindowTitleLength];
	bool			hasCoolSkateboard;
	bool			bootingFromEditor;

	uint32_t		numDisplayBuffers;
};

extern EngineGlobals g_engineGlobals;