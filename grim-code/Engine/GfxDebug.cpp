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
	static Vector3 s_pivot = Vector3(0.0f, 0.0f, 0.0f);
	static Vector3 s_viewDirection = Vector3(0.0f, 0.0f, 1.0f);
	static float s_distanceFromPivot = 1.0f;

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

		lwMatrix = Matrix::CreateLookAt(s_pivot + s_viewDirection * s_distanceFromPivot, s_pivot, Vector3::kUp);

		return lwMatrix;
	}

	void DebugCameraMovement(gfx::View& view, float dt)
	{
		if (grimInput::GetBool(grimInput::kSquare))
			s_isPivotMode = !s_isPivotMode;

		const Matrix newViewToWorld = s_isPivotMode ? DebugCam_PivotMode(view, dt) : DebugCam_FirstPersonMode(view, dt);
		gfx::UpdateView(view, newViewToWorld);
	}
}