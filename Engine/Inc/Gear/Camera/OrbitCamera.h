#pragma once

#ifndef _ORBITCAMERA_H_
#define _ORBITCAMERA_H_

#include<Gear/Core/Camera.h>
#include<Gear/Input/Mouse.h>
#include<Gear/Input/Keyboard.h>
#include<Gear/Utils/Math.h>

class OrbitCamera
{
public:

	OrbitCamera() = delete;

	OrbitCamera(const OrbitCamera&) = delete;

	void operator=(const OrbitCamera&) = delete;

	OrbitCamera(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& up, const float zoomSpeed);

	~OrbitCamera();

	void applyInput(const float dt);

	void rotateX(const float dTheta);

	void rotateY(const float dTheta);

private:

	DirectX::XMVECTOR eye;

	float currentRadius;

	float targetRadius;

	const float zoomSpeed;

	const DirectX::XMVECTOR up;

	int moveEventID;

	int scrollEventID;

};

#endif // !_ORBITCAMERA_H_
