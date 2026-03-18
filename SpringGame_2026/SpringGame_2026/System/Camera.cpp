#include "Camera.h"
#include "../System/Input.h"

namespace 
{
	constexpr float camera_near = 200.0f;//カメラのNear
	constexpr float camera_far = 1500.0f;//カメラのFar
	const Vector3 first_pos = { 0.0f,300.0f,-700.0f };
}

Camera::Camera(const Vector3& targetPos) :
	GameObject(first_pos),
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

void Camera::Update(const Vector3& targetPos, const Input input)
{
	//inputクラスから右スティックの入力を受け取って、値に応じて
	//カメラが回転するようにする

	//カメラのターゲットと位置をセットし続ける
	SetCameraPositionAndTarget_UpVecY(m_pos.ToDxLib(), targetPos.ToDxLib());
}

void Camera::Draw()
{

}
