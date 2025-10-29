#include<Gear/Core/MainCamera.h>

#include<Gear/Core/Internal/MainCameraInternal.h>

namespace
{
	struct MainCameraPrivate
	{

		DirectX::XMMATRIX projMatrix = {};

		DirectX::XMMATRIX viewMatrix = {};

		Gear::Core::MainCamera::Internal::Matrices matrices = {};

	}pvt;
}

DirectX::XMMATRIX Gear::Core::MainCamera::getProj()
{
	return pvt.projMatrix;
}

DirectX::XMMATRIX Gear::Core::MainCamera::getView()
{
	return pvt.viewMatrix;
}

DirectX::XMVECTOR Gear::Core::MainCamera::getEyePos()
{
	return pvt.matrices.eyePos;
}

void Gear::Core::MainCamera::setProj(const float fov, const float aspectRatio, const float zNear, const float zFar)
{
	setProj(DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, zNear, zFar));
}

void Gear::Core::MainCamera::setView(const DirectX::XMVECTOR& eyePos, const DirectX::XMVECTOR& focus, const DirectX::XMVECTOR& up)
{
	pvt.matrices.eyePos = eyePos;

	setView(DirectX::XMMatrixLookAtLH(eyePos, focus, up));
}

void Gear::Core::MainCamera::setProj(const DirectX::XMMATRIX& proj)
{
	pvt.matrices.prevViewProj = DirectX::XMMatrixTranspose(pvt.viewMatrix * pvt.projMatrix);

	pvt.projMatrix = proj;

	pvt.matrices.proj = DirectX::XMMatrixTranspose(pvt.projMatrix);

	pvt.matrices.viewProj = DirectX::XMMatrixTranspose(pvt.viewMatrix * pvt.projMatrix);
}

void Gear::Core::MainCamera::setView(const DirectX::XMMATRIX& view)
{
	pvt.matrices.prevViewProj = DirectX::XMMatrixTranspose(pvt.viewMatrix * pvt.projMatrix);

	pvt.viewMatrix = view;

	pvt.matrices.view = DirectX::XMMatrixTranspose(pvt.viewMatrix);

	pvt.matrices.viewProj = DirectX::XMMatrixTranspose(pvt.viewMatrix * pvt.projMatrix);

	pvt.matrices.normalMatrix = DirectX::XMMatrixInverse(nullptr, pvt.viewMatrix);
}

Gear::Core::MainCamera::Internal::Matrices Gear::Core::MainCamera::Internal::getMatrices()
{
	return pvt.matrices;
}