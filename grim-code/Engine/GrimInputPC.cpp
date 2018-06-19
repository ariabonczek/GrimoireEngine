#include "Precompiled.h"

#include <Engine/grimInput.h>

namespace grimInput
{
	gainput::InputManager s_inputManager;
	gainput::InputMap& GetInputMap()
	{
		static gainput::InputMap s_inputMap(s_inputManager, "Default");
		return s_inputMap;
	}

	static float Invert(float const value, void* userData)
	{
		return -value;
	}

	void InitAndSetupMap(int windowWidth, int windowHeight)
	{
		s_inputManager.SetDisplaySize(windowWidth, windowHeight);
		
		gainput::DeviceId mouseId = s_inputManager.CreateDevice<gainput::InputDeviceMouse>()
			;
		gainput::DeviceId keyboardId = s_inputManager.CreateDevice<gainput::InputDeviceKeyboard>();
		gainput::DeviceId padId = s_inputManager.CreateDevice<gainput::InputDevicePad>();

		// todo: make this dynamic and driven by data (and thus, editable via settings)
		gainput::InputMap& map = GetInputMap();
		
		// Game pad
		map.MapBool(kConfirm,	padId, gainput::PadButtonB);
		map.MapBool(kCancel,	padId, gainput::PadButtonA);
		map.MapBool(kSquare,	padId, gainput::PadButtonX);
		map.MapBool(kTriangle,	padId, gainput::PadButtonY);

		map.MapBool(kDpadLeft,	padId, gainput::PadButtonLeft);
		map.MapBool(kDpadRight, padId, gainput::PadButtonRight);
		map.MapBool(kDpadUp,	padId, gainput::PadButtonUp);
		map.MapBool(kDpadDown,	padId, gainput::PadButtonDown);

		map.MapBool(kL1,		padId, gainput::PadButtonL1);
		map.MapBool(kR1,		padId, gainput::PadButtonR1);

		map.MapFloat(kLeftReticleX,  padId, gainput::PadButtonLeftStickX);
		map.MapFloat(kLeftReticleY,  padId, gainput::PadButtonLeftStickY);
		map.MapFloat(kRightReticleX, padId, gainput::PadButtonRightStickX);
		map.MapFloat(kRightReticleY, padId, gainput::PadButtonRightStickY);

		map.MapFloat(kBackLeftReticle,  padId,	gainput::PadButtonAxis4);
		map.MapFloat(kBackRightReticle, padId,	gainput::PadButtonAxis5);
		 
		// Mouse
		map.MapBool(kConfirm,		 mouseId, gainput::MouseButtonLeft);
		map.MapFloat(kMouseReticleX, mouseId, gainput::MouseAxisX);
		map.MapFloat(kMouseReticleY, mouseId, gainput::MouseAxisY);

		map.MapBool(kLeftMouse,		 mouseId, gainput::MouseButtonLeft);
		map.MapBool(kRightMouse,	 mouseId, gainput::MouseButtonRight);
		map.MapBool(kMiddleMouse,	 mouseId, gainput::MouseButtonMiddle);

		// Keyboard
		map.MapBool(kConfirm, keyboardId, gainput::KeyEnd);
		map.MapBool(kLeftAlt, keyboardId, gainput::KeyAltL);

		map.MapFloat(kLeftReticleX, keyboardId, gainput::KeyD);
		map.MapFloat(kLeftReticleX, keyboardId, gainput::KeyA, 0.0f, 1.0f, Invert);
		
		map.MapFloat(kLeftReticleY, keyboardId, gainput::KeyW);
		map.MapFloat(kLeftReticleY, keyboardId, gainput::KeyS, 0.0f, 1.0f, Invert);

		map.MapFloat(kBackLeftReticle, keyboardId, gainput::KeyQ);
		map.MapFloat(kBackRightReticle, keyboardId, gainput::KeyE);
	}

	void Update()
	{
		s_inputManager.Update();
	}

#if PLATFORM_PC
	void HandleMessage(const MSG& msg)
	{
		s_inputManager.HandleMessage(msg);
	}
#endif

	bool GetBool(Button button)
	{
		return GetInputMap().GetBool(button);
	}

	bool GetBool(Key key)
	{
		return GetInputMap().GetBool(key);
	}

	bool GetBoolDown(Button button)
	{
		return GetInputMap().GetBoolIsNew(button);
	}

	bool GetBoolDown(Key key)
	{
		return GetInputMap().GetBoolIsNew(key);
	}

	bool GetBoolUp(Button button)
	{
		return GetInputMap().GetBoolWasDown(button);
	}

	bool GetBoolUp(Key key)
	{
		return GetInputMap().GetBoolWasDown(key);
	}

	float GetFloat(Axis axis)
	{
		return GetInputMap().GetFloat(axis);
	}

	float GetFloatDelta(Axis axis)
	{
		return GetInputMap().GetFloatDelta(axis);
	}
}