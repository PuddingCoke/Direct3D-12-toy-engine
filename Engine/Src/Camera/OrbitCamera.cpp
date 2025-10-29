#include<Gear/Camera/OrbitCamera.h>

#include<Gear/Core/MainCamera.h>

#include<Gear/Input/Mouse.h>

#include<Gear/Input/Keyboard.h>

Gear::OrbitCamera::OrbitCamera(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& up, const float zoomSpeed) :
	eye(DirectX::XMVector3Normalize(eye)), up(DirectX::XMVector3Normalize(up)), zoomSpeed(zoomSpeed)
{
	DirectX::XMStoreFloat(&targetRadius, DirectX::XMVector3Length(eye));

	currentRadius = targetRadius;

	moveEventID = Gear::Input::Mouse::addMoveEvent([this]()
		{
			if (Gear::Input::Mouse::getLeftDown())
			{
				const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(this->up, Gear::Input::Mouse::getDX() / 120.f);

				this->eye = DirectX::XMVector3Transform(this->eye, rotMat);

				float eyeUpAngle;

				DirectX::XMStoreFloat(&eyeUpAngle, DirectX::XMVector3AngleBetweenNormals(this->eye, this->up));

				const float destAngle = eyeUpAngle + Gear::Input::Mouse::getDY() / 120.f;

				float rotAngle = Gear::Input::Mouse::getDY() / 120.f;

				if (destAngle > Gear::Utils::Math::pi - Gear::Core::MainCamera::epsilon)
				{
					rotAngle = Gear::Utils::Math::pi - Gear::Core::MainCamera::epsilon - eyeUpAngle;
				}
				else if (destAngle < Gear::Core::MainCamera::epsilon)
				{
					rotAngle = Gear::Core::MainCamera::epsilon - eyeUpAngle;
				}

				const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Cross(this->up, this->eye);

				const DirectX::XMMATRIX upRotMat = DirectX::XMMatrixRotationAxis(upCrossLookDir, rotAngle);

				this->eye = DirectX::XMVector3Transform(this->eye, upRotMat);
			}
		});

	scrollEventID = Gear::Input::Mouse::addScrollEvent([this]()
		{
			targetRadius -= 0.5f * this->zoomSpeed * Gear::Input::Mouse::getWheelDelta();

			if (targetRadius < 0.1f)
			{
				targetRadius = 0.1f;
			}
		});
}

Gear::OrbitCamera::~OrbitCamera()
{
	Gear::Input::Mouse::removeMoveEvent(moveEventID);

	Gear::Input::Mouse::removeScrollEvent(scrollEventID);
}

void Gear::OrbitCamera::applyInput(const float dt)
{
	currentRadius = Gear::Utils::Math::lerp(currentRadius, targetRadius, Gear::Utils::Math::clamp(dt * 20.f, 0.f, 1.f));

	Gear::Core::MainCamera::setView(DirectX::XMVectorScale(eye, currentRadius), { 0,0,0 }, up);
}

void Gear::OrbitCamera::rotateX(const float dTheta)
{
	const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(up, dTheta);

	eye = DirectX::XMVector3Transform(eye, rotMat);
}

void Gear::OrbitCamera::rotateY(const float dTheta)
{
	float eyeUpAngle;

	DirectX::XMStoreFloat(&eyeUpAngle, DirectX::XMVector3AngleBetweenNormals(eye, up));

	const float destAngle = eyeUpAngle + dTheta;

	float rotAngle = dTheta;

	if (destAngle > Gear::Utils::Math::pi - Gear::Core::MainCamera::epsilon)
	{
		rotAngle = Gear::Utils::Math::pi - Gear::Core::MainCamera::epsilon - eyeUpAngle;
	}
	else if (destAngle < Gear::Core::MainCamera::epsilon)
	{
		rotAngle = Gear::Core::MainCamera::epsilon - eyeUpAngle;
	}

	const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Cross(up, eye);

	const DirectX::XMMATRIX upRotMat = DirectX::XMMatrixRotationAxis(upCrossLookDir, rotAngle);

	eye = DirectX::XMVector3Transform(eye, upRotMat);
}
