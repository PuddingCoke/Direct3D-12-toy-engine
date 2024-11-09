#include<Gear/Core/Camera.h>

DirectX::XMMATRIX Camera::projMatrix;

DirectX::XMMATRIX Camera::viewMatrix;

Camera::CameraMatrices Camera::matrices;

DirectX::XMMATRIX Camera::getProj()
{
	return projMatrix;
}

DirectX::XMMATRIX Camera::getView()
{
	return viewMatrix;
}

DirectX::XMVECTOR Camera::getEyePos()
{
	return matrices.eyePos;
}

void Camera::setProj(const float fov, const float aspectRatio, const float zNear, const float zFar)
{
	setProj(DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, zNear, zFar));
}

void Camera::setView(const DirectX::XMVECTOR& eyePos, const DirectX::XMVECTOR& focus, const DirectX::XMVECTOR& up)
{
	matrices.eyePos = eyePos;

	setView(DirectX::XMMatrixLookAtLH(eyePos, focus, up));
}

void Camera::setProj(const DirectX::XMMATRIX& proj)
{
	matrices.prevViewProj = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);

	projMatrix = proj;

	matrices.proj = DirectX::XMMatrixTranspose(projMatrix);

	matrices.viewProj = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);
}

void Camera::setView(const DirectX::XMMATRIX& view)
{
	matrices.prevViewProj = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);

	viewMatrix = view;

	matrices.view = DirectX::XMMatrixTranspose(viewMatrix);

	matrices.viewProj = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);

	matrices.normalMatrix = DirectX::XMMatrixInverse(nullptr, viewMatrix);
}
