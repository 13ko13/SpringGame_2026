#pragma once
#include "../Math/Vector3.h"
#include <vector>

class BackGround
{
public:
	BackGround();
	~BackGround();

	void Draw(const Vector3& cameraPos);

private:
	std::vector<int> m_skyTextures; //空のテクスチャIDを格納するベクター
};