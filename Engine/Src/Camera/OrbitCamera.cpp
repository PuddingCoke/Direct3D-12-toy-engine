#include<Gear/Camera/OrbitCamera.h>

OrbitCamera::OrbitCamera(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& up, const float zoomSpeed) :
	eye(DirectX::XMVector3Normalize(eye)), up(DirectX::XMVector3Normalize(up)), zoomSpeed(zoomSpeed)
{
	DirectX::XMStoreFloat(&targetRadius, DirectX::XMVector3Length(eye));

	currentRadius = targetRadius;

	moveEventID = Mouse::addMoveEvent([this]()
		{
			if (Mouse::getLeftDown())
			{
				const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(this->up, Mouse::getDX() / 120.f);

				this->eye = DirectX::XMVector3Transform(this->eye, rotMat);

				float eyeUpAngle;

				DirectX::XMStoreFloat(&eyeUpAngle, DirectX::XMVector3AngleBetweenNormals(this->eye, this->up));

				const float destAngle = eyeUpAngle + Mouse::getDY() / 120.f;

				float rotAngle = Mouse::getDY() / 120.f;

				if (destAngle > Math::pi - Camera::epsilon)
				{
					rotAngle = Math::pi - Camera::epsilon - eyeUpAngle;
				}
				else if (destAngle < Camera::epsilon)
				{
					rotAngle = Camera::epsilon - eyeUpAngle;
				}

				const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Cross(this->up, this->eye);

				const DirectX::XMMATRIX upRotMat = DirectX::XMMatrixRotationAxis(upCrossLookDir, rotAngle);

				this->eye = DirectX::XMVector3Transform(this->eye, upRotMat);
			}
		});

	scrollEventID = Mouse::addScrollEvent([this]()
		{
			targetRadius -= 0.5f * this->zoomSpeed * Mouse::getWheelDelta();

			if (targetRadius < 0.1f)
			{
				targetRadius = 0.1f;
			}
		});
}

OrbitCamera::~OrbitCamera()
{
	Mouse::removeMoveEvent(moveEventID);

	Mouse::removeScrollEvent(scrollEventID);
}

void OrbitCamera::applyInput(const float dt)
{
	currentRadius = Math::lerp(currentRadius, targetRadius, Math::clamp(dt * 20.f, 0.f, 1.f));

	Camera::setView(DirectX::XMVectorScale(eye, currentRadius), { 0,0,0 }, up);
}

void OrbitCamera::rotateX(const float dTheta)
{
	const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(up, dTheta);

	eye = DirectX::XMVector3Transform(eye, rotMat);
}

void OrbitCamera::rotateY(const float dTheta)
{
	float eyeUpAngle;

	DirectX::XMStoreFloat(&eyeUpAngle, DirectX::XMVector3AngleBetweenNormals(eye, up));

	const float destAngle = eyeUpAngle + dTheta;

	float rotAngle = dTheta;

	if (destAngle > Math::pi - Camera::epsilon)
	{
		rotAngle = Math::pi - Camera::epsilon - eyeUpAngle;
	}
	else if (destAngle < Camera::epsilon)
	{
		rotAngle = Camera::epsilon - eyeUpAngle;
	}

	const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Cross(up, eye);

	const DirectX::XMMATRIX upRotMat = DirectX::XMMatrixRotationAxis(upCrossLookDir, rotAngle);

	eye = DirectX::XMVector3Transform(eye, upRotMat);
}
