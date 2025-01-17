#include<Gear/Camera/FPSCamera.h>

FPSCamera::FPSCamera(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& lookDir, const DirectX::XMVECTOR& up, const float moveSpeed):
	eye(eye), lookDir(lookDir), up(up), moveSpeed(moveSpeed)
{
	moveEventID = Mouse::addMoveEvent([this]()
		{
			if (Mouse::getLeftDown())
			{
				const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(this->up, Mouse::getDX() / 120.f);

				this->lookDir = DirectX::XMVector3Transform(this->lookDir, rotMat);

				const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(this->up, this->lookDir));

				float lookUpAngle;

				DirectX::XMStoreFloat(&lookUpAngle, DirectX::XMVector3AngleBetweenNormals(this->lookDir, this->up));

				const float destAngle = lookUpAngle - Mouse::getDY() / 120.f;

				float rotAngle = -Mouse::getDY() / 120.f;

				if (destAngle > Math::pi - Camera::epsilon)
				{
					rotAngle = Math::pi - Camera::epsilon - lookUpAngle;
				}
				else if (destAngle < Camera::epsilon)
				{
					rotAngle = Camera::epsilon - lookUpAngle;
				}

				const DirectX::XMMATRIX lookDirRotMat = DirectX::XMMatrixRotationAxis(upCrossLookDir, rotAngle);

				this->lookDir = DirectX::XMVector3Transform(this->lookDir, lookDirRotMat);
			}
		});
}

FPSCamera::~FPSCamera()
{
	Mouse::removeMoveEvent(moveEventID);
}

void FPSCamera::applyInput(const float dt)
{
	if (Keyboard::getKeyDown(Keyboard::W))
	{
		eye = DirectX::XMVectorAdd(eye, DirectX::XMVectorScale(lookDir, dt * moveSpeed));
	}

	if (Keyboard::getKeyDown(Keyboard::S))
	{
		eye = DirectX::XMVectorAdd(eye, DirectX::XMVectorScale(lookDir, -dt * moveSpeed));
	}

	if (Keyboard::getKeyDown(Keyboard::A))
	{
		const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up, lookDir));
		eye = DirectX::XMVectorAdd(eye, DirectX::XMVectorScale(upCrossLookDir, -dt * moveSpeed));
	}

	if (Keyboard::getKeyDown(Keyboard::D))
	{
		const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up, lookDir));
		eye = DirectX::XMVectorAdd(eye, DirectX::XMVectorScale(upCrossLookDir, dt * moveSpeed));
	}

	Camera::setView(eye, DirectX::XMVectorAdd(eye, lookDir), up);
}

DirectX::XMVECTOR FPSCamera::getEyePos() const
{
	return eye;
}

DirectX::XMVECTOR FPSCamera::getLookDir() const
{
	return lookDir;
}

DirectX::XMVECTOR FPSCamera::getUpVector() const
{
	return up;
}
