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
	if (input.GetRightStickDir().m_x > 0.0f)
	{
		m_angle += input.GetRightStickDir().m_x * 0.5f;//回転速度は0.05
		m_pos.m_x += m_angle;//カメラの位置を更新する
		m_pos.m_z += m_angle;
	}
	else if(input.GetRightStickDir().m_x < 0.0f)
	{
		m_angle -= input.GetRightStickDir().m_x * 0.5f;//回転速度は0.05
		m_pos.m_x -= m_angle;//カメラの位置を更新する
		m_pos.m_z -= m_angle;
	}
	else
	{
		m_angle = 0.0f;//スティックがニュートラルのときは回転しないようにする
	}

	DrawFormatString(0.0f, 0.0f, 0xffffff, "RStickX : %f", input.GetRightStickDir().m_x);

	
	

	//カメラのターゲットと位置をセットし続ける
	SetCameraPositionAndTarget_UpVecY(m_pos.ToDxLib(), targetPos.ToDxLib());
}

void Camera::Draw()
{
	//DrawFormatString(0.0f, 0.0f, 0xffffff, "cameraPosX : %f,Y : %f,Y : %f", m_pos.m_x,m_pos.m_y,m_pos.m_z);
}
