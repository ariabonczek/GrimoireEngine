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
		
		kNumButtons
	};

	enum Axis  // floating point
	{
		kLeftReticleX = kNumButtons, // can't overlap these values
		kLeftReticleY,
		kRightReticleX,
		kRightReticleY,

		kBackLeftReticle,
		kBackRightReticle,

		kMouseReticleX,
		kMouseReticleY
	};

#if PLATFORM_PC
	void HandleMessage(const MSG& msg);
#endif

	void InitAndSetupMap(int windowWidth, int windowHeight);
	void Update();

	bool  GetBool(Button button);
	float GetFloat(Axis axis);
}