#include<Gear/Camera/FPSCamera.h>

#include<Gear/Core/MainCamera.h>

#include<Gear/Input/Mouse.h>

#include<Gear/Input/Keyboard.h>

Gear::FPSCamera::FPSCamera(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& lookDir, const DirectX::XMVECTOR& up, const float moveSpeed):
	eye(eye), lookDir(lookDir), up(up), moveSpeed(moveSpeed)
{
	this->lookDir = DirectX::XMVector3Normalize(this->lookDir);

	moveEventID = Gear::Input::Mouse::addMoveEvent([this]()
		{
			if (Gear::Input::Mouse::getLeftDown())
			{
				const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(this->up, Gear::Input::Mouse::getDX() / 120.f);

				this->lookDir = DirectX::XMVector3Transform(this->lookDir, rotMat);

				const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(this->up, this->lookDir));

				float lookUpAngle;

				DirectX::XMStoreFloat(&lookUpAngle, DirectX::XMVector3AngleBetweenNormals(this->lookDir, this->up));

				const float destAngle = lookUpAngle - Gear::Input::Mouse::getDY() / 120.f;

				float rotAngle = -Gear::Input::Mouse::getDY() / 120.f;

				if (destAngle > Gear::Utils::Math::pi - Gear::Core::MainCamera::epsilon)
				{
					rotAngle = Gear::Utils::Math::pi - Gear::Core::MainCamera::epsilon - lookUpAngle;
				}
				else if (destAngle < Gear::Core::MainCamera::epsilon)
				{
					rotAngle = Gear::Core::MainCamera::epsilon - lookUpAngle;
				}

				const DirectX::XMMATRIX lookDirRotMat = DirectX::XMMatrixRotationAxis(upCrossLookDir, rotAngle);

				this->lookDir = DirectX::XMVector3Transform(this->lookDir, lookDirRotMat);
			}
		});
}

Gear::FPSCamera::~FPSCamera()
{
	Gear::Input::Mouse::removeMoveEvent(moveEventID);
}

void Gear::FPSCamera::applyInput(const float dt)
{
	if (Gear::Input::Keyboard::getKeyDown(Gear::Input::Keyboard::W))
	{
		eye = DirectX::XMVectorAdd(eye, DirectX::XMVectorScale(lookDir, dt * moveSpeed));
	}

	if (Gear::Input::Keyboard::getKeyDown(Gear::Input::Keyboard::S))
	{
		eye = DirectX::XMVectorAdd(eye, DirectX::XMVectorScale(lookDir, -dt * moveSpeed));
	}

	if (Gear::Input::Keyboard::getKeyDown(Gear::Input::Keyboard::A))
	{
		const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up, lookDir));
		eye = DirectX::XMVectorAdd(eye, DirectX::XMVectorScale(upCrossLookDir, -dt * moveSpeed));
	}

	if (Gear::Input::Keyboard::getKeyDown(Gear::Input::Keyboard::D))
	{
		const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up, lookDir));
		eye = DirectX::XMVectorAdd(eye, DirectX::XMVectorScale(upCrossLookDir, dt * moveSpeed));
	}

	Gear::Core::MainCamera::setView(eye, DirectX::XMVectorAdd(eye, lookDir), up);
}

DirectX::XMVECTOR Gear::FPSCamera::getEyePos() const
{
	return eye;
}

DirectX::XMVECTOR Gear::FPSCamera::getLookDir() const
{
	return lookDir;
}

DirectX::XMVECTOR Gear::FPSCamera::getUpVector() const
{
	return up;
}
