#pragma once

#ifndef _FPSCAMERA_H_
#define _FPSCAMERA_H_

#include<Gear/Core/Camera.h>
#include<Gear/Input/Mouse.h>
#include<Gear/Input/Keyboard.h>
#include<Gear/Utils/Math.h>

class FPSCamera
{
public:

	FPSCamera() = delete;

	FPSCamera(const FPSCamera&) = delete;

	void operator=(const FPSCamera&) = delete;

	FPSCamera(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& lookDir, const DirectX::XMVECTOR& up, const float moveSpeed);

	~FPSCamera();

	void applyInput(const float& dt);

private:

	const float moveSpeed;

	DirectX::XMVECTOR eye;

	DirectX::XMVECTOR lookDir;

	DirectX::XMVECTOR up;

	int eventID;

};

#endif // !_FPSCAMERA_H_
