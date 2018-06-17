#pragma once

#include <Shared/Math/Math.h>

namespace gfx
{
	struct ViewPort
	{
		ViewPort();

		float topLeftX, topLeftY;
		float width, height; // in pixels
		float minDepth, maxDepth;
	};

	struct View
	{
		View();

		ViewPort viewPort;

		Matrix worldToView;
		Matrix viewToWorld;

		Matrix viewToProj;
		Matrix projToView;

		Matrix worldToProj;
		Matrix projToWorld;

		float nearZ;
		float farZ;
		float aspectRatio;
		float fovY;

		float nearHeight;
		float farHeight;
	};

	View MakeView(float fovY, float viewportWidth, float viewPortHeight, float nearZ, float farZ, Matrix lwMatrix);
	void UpdateView(View& view, Matrix newLwMatrix);

	struct RenderView
	{
		Matrix worldToView;
		Matrix viewToWorld;

		Matrix viewToProj;
		Matrix projToView;

		Matrix worldToProj;
		Matrix projToWorld;

		Vector4 eyePosition;
	};

	RenderView RenderViewFromView(const View& view);
}