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

	//エフェクトの再生ハンドルを取得する
	int GetPlayingHandle() const { return m_playingHandle; }

	//エフェクトが再生中かを判定する
	bool IsPlaying() const;

private:

protected:
	//現在再生中のエフェクトのハンドル
	int m_playingHandle = -1;
	//リソースハンドル
	int m_resourceHandle = -1;
};

