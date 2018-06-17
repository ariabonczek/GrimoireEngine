#include "Precompiled.h"

#include "View.h"

namespace gfx
{
	ViewPort::ViewPort():
		topLeftX(0.0f),
		topLeftY(0.0f),
		width(64.0f),
		height(64.0f),
		minDepth(0.0f),
		maxDepth(1.0f)
	{}

	View::View():
		viewPort(),
		worldToView(Matrix::kIdentity),
		viewToWorld(Matrix::kIdentity),
		viewToProj(Matrix::kIdentity),
		projToView(Matrix::kIdentity),
		worldToProj(Matrix::kIdentity),
		projToWorld(Matrix::kIdentity),
		nearZ(0.0f), farZ(0.0f),
		aspectRatio(0.0f), fovY(0.0f),
		nearHeight(0.0f), farHeight(0.0f)
	{}

	View MakeView(float fovY, float viewportWidth, float viewPortHeight, float nearZ, float farZ, Matrix lwMatrix)
	{
		View view;

		view.viewPort.width = viewportWidth;
		view.viewPort.height = viewPortHeight;

		view.fovY = fovY;
		view.aspectRatio = viewportWidth / viewPortHeight;
		view.nearZ = nearZ;
		view.farZ = farZ;

		view.nearHeight = 2.0f * nearZ * tanf(0.5f * fovY);
		view.farHeight = 2.0f * farZ * tanf(0.5f * fovY);

		view.viewToWorld = lwMatrix;
		view.worldToView = Matrix::Inverse(lwMatrix);

		view.viewToProj = Matrix::CreatePerspectiveFov(fovY, view.aspectRatio, nearZ, farZ);
		view.projToView = Matrix::Inverse(view.viewToProj);

		view.worldToProj = view.worldToView * view.viewToProj;
		view.projToWorld = Matrix::Inverse(view.worldToProj);

		return view;
	}

	void UpdateView(View& view, Matrix newLwMatrix)
	{
		view.viewToWorld = newLwMatrix;
		view.worldToView = Matrix::Inverse(newLwMatrix);

		view.worldToProj = view.worldToView * view.viewToProj;
		view.projToWorld = Matrix::Inverse(view.worldToProj);
	}

	RenderView RenderViewFromView(const View& view)
	{
		RenderView renderView;

		renderView.worldToView = view.worldToView;
		renderView.viewToWorld = view.viewToWorld;
		renderView.viewToProj = view.viewToProj;
		renderView.projToView = view.projToView;
		renderView.worldToProj = view.worldToProj;
		renderView.projToWorld = view.projToWorld;

		Vector4 eyePosition = view.viewToWorld.GetRowW();
		eyePosition.w = 0.0f;
		renderView.eyePosition = eyePosition;

		return renderView;
	}
}