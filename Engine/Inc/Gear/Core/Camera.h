#pragma once

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include<Gear/Utils/Math.h>

class Camera
{
public:

	Camera(const Camera&) = delete;

	void operator=(const Camera&) = delete;

	static DirectX::XMMATRIX getProj();

	static DirectX::XMMATRIX getView();

	static DirectX::XMVECTOR getEyePos();

	static void setProj(const float fov, const float aspectRatio, const float zNear, const float zFar);

	static void setView(const DirectX::XMVECTOR& eyePos, const DirectX::XMVECTOR& focus, const DirectX::XMVECTOR& up);

	static void setProj(const DirectX::XMMATRIX& proj);

	static void setView(const DirectX::XMMATRIX& view);

	static constexpr float epsilon = 0.01f;

private:

	friend class RenderEngine;

	static DirectX::XMMATRIX projMatrix;

	static DirectX::XMMATRIX viewMatrix;

	static struct CameraMatrices
	{
		DirectX::XMMATRIX proj;
		DirectX::XMMATRIX view;
		DirectX::XMVECTOR eyePos;
		DirectX::XMMATRIX prevViewProj;
		DirectX::XMMATRIX viewProj;
		DirectX::XMMATRIX normalMatrix;
	} matrices;

};

#endif // !_CAMERA_H_