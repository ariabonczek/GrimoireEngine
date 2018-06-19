#include "Precompiled.h"

#include <Engine/GfxDebug.h>

#include <Engine/View.h>
#include <Engine/GrimInput.h>

namespace debug
{
	static float s_xSpeed = 10.0f;
	static float s_ySpeed = 20.0f;
	static float s_zSpeed = 10.0f;

	static float s_lookSpeed = 0.333f;
	
	static bool s_isPivotMode = false;
	static bool s_isMouseMode = false;

	static Vector3 s_pivot = Vector3(0.0f, 0.0f, 0.0f);
	static Vector3 s_viewDirection = Vector3(0.0f, 0.0f, 1.0f);
	static float s_distanceFromPivot = 3.0f;

	static Matrix DebugCam_FirstPersonMode(const gfx::View& view, float dt)
	{
		Matrix lwMatrix = view.viewToWorld;

		// Camera flight
		{
			Matrix rotation = Matrix(
				Vector4::Normalized(lwMatrix.GetRowX()),
				Vector4::Normalized(lwMatrix.GetRowY()),
				Vector4::Normalized(lwMatrix.GetRowZ()),
				Vector4::kUnitWAxis);

			float leftReticleX = grimInput::GetFloat(grimInput::kLeftReticleX);
			float leftReticleY = grimInput::GetFloat(grimInput::kLeftReticleY);

			float deltaX = leftReticleX * s_xSpeed;// - s_lastLeftReticleX;
			float deltaZ = leftReticleY * s_zSpeed;// - s_lastLeftReticleY;

			float leftY = grimInput::GetFloat(grimInput::kBackLeftReticle);
			float rightY = grimInput::GetFloat(grimInput::kBackRightReticle);

			float deltaY = leftY > 0.0f ? -leftY : rightY > 0.0f ? rightY : 0.0f;
			deltaY *= s_ySpeed;

			Vector4 motion = Vector4(deltaX, deltaY, deltaZ, 0.0f) * dt;
			motion = motion * rotation;

			lwMatrix.SetRowW(lwMatrix.GetRowW() + motion);
		}

		// Camera rotation
		{
			float rightReticleX = grimInput::GetFloat(grimInput::kRightReticleX);
			float rightReticleY = -grimInput::GetFloat(grimInput::kRightReticleY);

			float deltaX = (rightReticleX)* s_lookSpeed;
			float deltaY = (rightReticleY)* s_lookSpeed;

			// X movement Yaw (rotate up axis)
			Vector3 axisYaw = Vector3::kUp;
			Quaternion rotateYaw = Quaternion::CreateFromAxisAngle(axisYaw, deltaX);

			// Y movement pitch (rotate right axis)
			Vector4 axisPitch_vec4 = Vector4::Normalized(lwMatrix.GetRowX());
			Vector3 axisPitch = Vector3(axisPitch_vec4.x, axisPitch_vec4.y, axisPitch_vec4.z);
			Quaternion rotatePitch = Quaternion::CreateFromAxisAngle(axisPitch, deltaY);

			Quaternion combined = (rotatePitch * rotateYaw).Normalized();
			Matrix combinedMatrix = Matrix::CreateFromQuaternion(combined);

			lwMatrix.SetRowX((lwMatrix.GetRowX() * combinedMatrix).Normalized());
			lwMatrix.SetRowY((lwMatrix.GetRowY() * combinedMatrix).Normalized());
			lwMatrix.SetRowZ((lwMatrix.GetRowZ() * combinedMatrix).Normalized());
		}

		return lwMatrix;
	}

	static Matrix DebugCam_PivotMode(const gfx::View& view, float dt)
	{
		Matrix lwMatrix = view.viewToWorld;

		s_viewDirection = lwMatrix.GetRowZ().Normalized().AsVector3();

		bool anyMovement = false;

		// always looks at pivot
		// left stick moves the pivot
		// right stick Y zooms camera in/out, right Stick X rotates around pivot

		// pivot motion
		{
			Matrix rotation = Matrix(
				Vector4::Normalized(lwMatrix.GetRowX()),
				Vector4::Normalized(lwMatrix.GetRowY()),
				Vector4::Normalized(lwMatrix.GetRowZ()),
				Vector4::kUnitWAxis);

			float leftReticleX = grimInput::GetFloat(grimInput::kLeftReticleX);
			float leftReticleY = grimInput::GetFloat(grimInput::kLeftReticleY);

			float deltaX = leftReticleX * s_xSpeed;// - s_lastLeftReticleX;
			float deltaZ = leftReticleY * s_zSpeed;// - s_lastLeftReticleY;

			float leftY = grimInput::GetFloat(grimInput::kBackLeftReticle);
			float rightY = grimInput::GetFloat(grimInput::kBackRightReticle);

			float deltaY = leftY > 0.0f ? -leftY : rightY > 0.0f ? rightY : 0.0f;
			deltaY *= s_ySpeed;

			Vector4 motion = Vector4(deltaX, deltaY, deltaZ, 0.0f) * dt;
			motion = motion * rotation;
			s_pivot = s_pivot + motion.AsVector3();
		}

		// pivot zoom
		{
			float rightReticleX = grimInput::GetFloat(grimInput::kRightReticleX);
			float rightReticleY = grimInput::GetFloat(grimInput::kRightReticleY);

			float deltaDist = rightReticleY * s_lookSpeed * dt;
			float deltaRot = rightReticleX * s_lookSpeed * dt;

			s_distanceFromPivot = s_distanceFromPivot + deltaDist;

			Vector3 axisYaw = Vector3::kUp;
			Quaternion rotateYaw = Quaternion::CreateFromAxisAngle(axisYaw, deltaRot).Normalized();
			Matrix rotate = Matrix::CreateFromQuaternion(rotateYaw);

			s_viewDirection = s_viewDirection * rotate;
		}

		if(anyMovement)
			lwMatrix = Matrix::CreateLookAt(s_pivot + s_viewDirection * s_distanceFromPivot, s_pivot, Vector3::kUp);

		return lwMatrix;
	}

	// always looks at pivot
	// left stick moves the pivot
	// right stick Y zooms camera in/out, right Stick X rotates around pivot
	static Matrix DebugCam_MouseMode(const gfx::View& view, float dt)
	{
		Matrix lwMatrix = view.viewToWorld;

		if (grimInput::GetBoolDown(grimInput::kLeftAlt))
		{
			Vector3 eyePosition = lwMatrix.GetRowW().AsVector3();
			Vector3 eyeToPivot = s_pivot - eyePosition;

			s_distanceFromPivot = eyeToPivot.Length();
			s_viewDirection = eyeToPivot.Normalized();
		}
		
		bool anyMovement = false;

		float deltaX = grimInput::GetFloatDelta(grimInput::kMouseReticleX) * dt * 60; // factoring in dt to prevent framerate issues, multiplying by 60 to normalize it
		float deltaY = grimInput::GetFloatDelta(grimInput::kMouseReticleY) * dt * 60; // factoring in dt to prevent framerate issues, multiplying by 60 to normalize it

		// mouse look
		if(grimInput::GetBool(grimInput::kLeftMouse))
		{
			// X movement Yaw (rotate up axis)
			Vector3 axisYaw = Vector3::kUp;
			Quaternion rotateYaw = Quaternion::CreateFromAxisAngle(axisYaw, deltaX * 360);

			// Y movement pitch (rotate right axis)
			Vector3 axisPitch = Vector4::Normalized(lwMatrix.GetRowX()).AsVector3();
			Quaternion rotatePitch = Quaternion::CreateFromAxisAngle(axisPitch, deltaY * 360);

			Quaternion combined = (rotatePitch * rotateYaw).Normalized();
			Matrix combinedMatrix = Matrix::CreateFromQuaternion(combined);

			s_viewDirection = s_viewDirection * combinedMatrix;
			anyMovement = true;
		}

		// pivot zoom
		if (grimInput::GetBool(grimInput::kRightMouse))
		{
			s_distanceFromPivot -= s_distanceFromPivot * deltaX;
			anyMovement = true;
		}

		// pivot zoom
		if (grimInput::GetBool(grimInput::kMiddleMouse))
		{
			Vector3 cameraRight = lwMatrix.GetRowX().Normalized().AsVector3();
			Vector3 cameraUp = lwMatrix.GetRowY().Normalized().AsVector3();

			s_pivot -= cameraRight * deltaX * s_distanceFromPivot;
			s_pivot += cameraUp * deltaY * s_distanceFromPivot;

			anyMovement = true;
		}

		if (anyMovement)
			lwMatrix = Matrix::CreateLookAt(s_pivot - s_viewDirection * s_distanceFromPivot, s_pivot, Vector3::kUp);

		return lwMatrix;
	}

	void DebugCameraMovement(gfx::View& view, float dt)
	{
		if (grimInput::GetBool(grimInput::kSquare))
			s_isPivotMode = !s_isPivotMode;

		s_isMouseMode = grimInput::GetBool(grimInput::kLeftAlt);

		Matrix newViewToWorld = Matrix::kIdentity;
		if (s_isMouseMode)
			newViewToWorld = DebugCam_MouseMode(view, dt);
		else
			newViewToWorld = s_isPivotMode ? DebugCam_PivotMode(view, dt) : DebugCam_FirstPersonMode(view, dt);

		gfx::UpdateView(view, newViewToWorld);
	}
}