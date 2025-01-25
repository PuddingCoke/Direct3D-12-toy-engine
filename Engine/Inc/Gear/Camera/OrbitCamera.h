﻿#pragma once

#ifndef _ORBITCAMERA_H_
#define _ORBITCAMERA_H_

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

	uint64_t moveEventID;

	uint64_t scrollEventID;

};

#endif // !_ORBITCAMERA_H_