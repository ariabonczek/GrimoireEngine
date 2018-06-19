#pragma once

#if defined PLATFORM_PC
#define GAINPUT_PLATFORM_WIN

#pragma warning(push, 0)        
#include <Shared/gainput/gainput.h>
#pragma warning(pop)
#endif

#include <stdint.h>

namespace grimInput
{
	enum Button // binary state
	{
		kConfirm,
		kCancel,
		kSquare,
		kTriangle,

		kDpadLeft,
		kDpadRight,
		kDpadUp,
		kDpadDown,

		kL1,
		kR1,

		kLeftMouse,
		kRightMouse,
		kMiddleMouse,
		
		kLastButton
	};

	enum Axis  // floating point
	{
		kLeftReticleX = kLastButton, // can't overlap these values
		kLeftReticleY,
		kRightReticleX,
		kRightReticleY,

		kBackLeftReticle,
		kBackRightReticle,

		kMouseReticleX,
		kMouseReticleY,

		kLastAxis
	};

	enum Key
	{
		kLeftAlt = kLastAxis,

		kLastKey
	};

#if PLATFORM_PC
	void HandleMessage(const MSG& msg);
#endif

	void InitAndSetupMap(int windowWidth, int windowHeight);
	void Update();

	bool  GetBool(Button button);
	bool  GetBool(Key button);
	bool  GetBoolDown(Button button);
	bool  GetBoolDown(Key button);
	bool  GetBoolUp(Button button);
	bool  GetBoolUp(Key button);

	float GetFloat(Axis axis);
	float GetFloatDelta(Axis axis);
}