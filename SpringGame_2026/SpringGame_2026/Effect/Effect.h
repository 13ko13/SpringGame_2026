#pragma once
#include "../GameObjects/GameObject.h"
#include "../Math/Vector3.h"

class Effect : public GameObject
{
public:	
	Effect(const Vector3& pos);
	virtual ~Effect();

	virtual void Update() override;
	virtual void Draw() override;

private:

protected:
	//現在再生中のエフェクトのハンドル
	int m_playingHandle = -1;
};

