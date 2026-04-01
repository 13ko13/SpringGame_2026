#include "Camera.h"
#include "../System/Input.h"
#include "../Math/Matrix4x4.h"

namespace 
{
	constexpr float camera_near = 200.0f;//カメラのNear
	constexpr float camera_far = 3000.0f;//カメラのFar
	const Vector3 first_pos = { 0.0f,300.0f,-700.0f };
	//注視点からカメラ位置に向かうベクトル
	const Vector3 target_to_camera = { 0.0f,0.0f,-935.0f };
	//右スティックを動かしたときのカメラの回転角の増減量
	constexpr float camera_rotate_speed = 0.04f;
	//カメラが回転するまでのデッドゾーン
	constexpr float camera_rot_dead_zone = 0.5f;

	constexpr float fov = DX_PI_F / 3.0f;//カメラの視野角
}

Camera::Camera(const Vector3& targetPos) :
	GameObject(first_pos)
{
	//カメラの設定
	//プレイヤーの位置を取得して、そこをカメラのターゲットにする
	SetCameraPositionAndTarget_UpVecY(m_pos.ToDxLib(), targetPos.ToDxLib());

	//カメラの視野角を設定する(180/3)
	SetupCamera_Perspective(fov);
	//カメラの遠近感の範囲を設定する
	SetCameraNearFar(camera_near, camera_far);
	m_targetPos = targetPos;
}

Camera::~Camera()
{

}

void Camera::Update()
{

}

void Camera::Update(const Vector3& targetPos, const Input input)
{
	//右スティックの入力に応じてカメラの回転角を更新する
	if (input.GetRightStickDir().m_x > camera_rot_dead_zone)
	{
		m_angleY -= camera_rotate_speed;
	}
	else if(input.GetRightStickDir().m_x < -camera_rot_dead_zone)
	{
		m_angleY += camera_rotate_speed;
	}

	//上下入力時のカメラの回転角の更新
	if (input.GetRightStickDir().m_y > camera_rot_dead_zone)
	{
		m_angleX += camera_rotate_speed;
	}
	else if (input.GetRightStickDir().m_y < -camera_rot_dead_zone)
	{
		m_angleX -= camera_rotate_speed;
	}

	//カメラの回転角を制限する
	if (m_angleX > DX_PI_F / 3.0f) m_angleX = DX_PI_F / 3.0f;//真下向くときは60度以上回転しないようにする
	if (m_angleX < -DX_PI_F / 6.0f) m_angleX = -DX_PI_F / 6.0f;//-30度以上回転しないようにする

	//カメラの注視点
	Vector3 target = targetPos;
	//見ている方向から回転行列を作る
	Matrix4x4 rotMtxY = Matrix4x4::RotationY(m_angleY);
	Matrix4x4 rotMtxX = Matrix4x4::RotationX(m_angleX);
	//回転行列を掛け合わせる
	Matrix4x4 rotMtx = rotMtxX * rotMtxY;
	//注視点からカメラの位置に向かうベクトルに回転行列をかけることで
	//回転させたベクトルを求める
	Vector3 targetToCamera = rotMtx.Transform(target_to_camera);
	Vector3 endCameraPos = target + targetToCamera;

	//m_posからendCameraPosへのベクトル
	Vector3 tempCameraPos = endCameraPos - m_pos;
	tempCameraPos *= 0.2f;//カメラの移動速度を調整するために、ベクトルを0.1倍する

	//回転させたカメラへのベクトルとターゲットの位置を足して、
	//カメラの位置を求める
	m_pos += tempCameraPos;

	//ターゲットの位置も補間する
	Vector3 targetDelta = targetPos - m_targetPos;
	targetDelta *= 0.2f;
	m_targetPos += targetDelta;

	//カメラの位置とターゲットの位置をセットする
	SetCameraPositionAndTarget_UpVecY(m_pos.ToDxLib(), m_targetPos.ToDxLib());

#ifdef _DEBUG
	//DrawFormatString((int)0.0f, (int)0.0f, 0xffffff, "rightStickX : %f,Y : %f", input.GetRightStickDir().m_x, input.GetRightStickDir().m_y);
#endif // _DEBUG
}

void Camera::Draw()
{
#ifdef _DEBUG
	//DrawFormatString((int)0.0f, (int)30.0f, 0xffffff, "angleX : %f,Y : %f", m_angleX,m_angleY);
#endif // _DEBUG
}
