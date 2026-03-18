#include "Camera.h"

namespace 
{
	const float camera_near = 200.0f;//カメラのNear
	const float camera_far = 1500.0f;//カメラのFar
}

Camera::Camera(const Vector3& targetPos) :
	GameObject({0.0f,0.0f,0.0f}),
	m_angle(0.0f)
{
	//カメラの設定
	//プレイヤーの位置を取得して、そこをカメラのターゲットにする
	SetCameraPositionAndTarget_UpVecY(m_pos.ToDxLib(), targetPos.ToDxLib());

	//カメラの視野角を設定する(180/3)
	SetupCamera_Perspective(DX_PI_F / 3.0f);
	//カメラの遠近感の範囲を設定する
	SetCameraNearFar(camera_near, camera_far);
}

Camera::~Camera()
{
}

void Camera::Update()
{

}

void Camera::Update(const Vector3& targetPos)
{
	SetCameraPositionAndTarget_UpVecY(m_pos.ToDxLib(), targetPos.ToDxLib());
}

void Camera::Draw()
{
}
