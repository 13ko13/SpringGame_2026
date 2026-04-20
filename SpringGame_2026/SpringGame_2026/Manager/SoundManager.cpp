#define NOMINMAX
#include "SoundManager.h"
#include <DxLib.h>
#include <algorithm>
#include "../Loader/ResourceLoader.h"

namespace
{
	//タイトルBGMの音量
	constexpr int title_bgm_volume = 180;

	//決定音の音量
	constexpr int desition_volume = 255;
}

SoundManager::SoundManager()
{

}

SoundManager& SoundManager::GetInstance()
{
	//staticで宣言してそれを返す
	static SoundManager instance;
	return instance;
}

SoundManager::~SoundManager()
{
	//全ての音を停止する
	StopAll();
}

void SoundManager::Init()
{
	//リソースローダーのインスタンスを取得
	auto& loader = ResourceLoader::GetInstance();

	//それぞれのハンドルを取得して設定する
	auto& decisionData = m_sounds[SoundType::Decision];//決定音のデータを取得
	decisionData.handle = loader.GetSound(ResourceLoader::SoundID::Decision);//リソースローダーから決定音のハンドルを取得して設定
	decisionData.loaded = true;//ロード済みフラグを立てる
	decisionData.volume = desition_volume;//音量を最大にする
	decisionData.isLoop = false;//ループしない
	ChangeVolumeSoundMem(decisionData.volume, decisionData.handle);//音量を変更する

	auto& titleBgm = m_sounds[SoundType::TitleBgm];
	titleBgm.handle = loader.GetSound(ResourceLoader::SoundID::TitleBgm);//リソースローダーからタイトルのBGMのハンドルを取得して設定
	titleBgm.loaded = true;//ロード済みフラグを立てる
	titleBgm.volume = title_bgm_volume;//音量を最大にする
	titleBgm.isLoop = true;//ループする
	ChangeVolumeSoundMem(titleBgm.volume, titleBgm.handle);//音量を変更する

	auto& gameBgm = m_sounds[SoundType::GameBgm];
	gameBgm.handle = loader.GetSound(ResourceLoader::SoundID::GameBgm);//リソースローダーからゲームシーンのBGMのハンドルを取得して設定
	gameBgm.loaded = true;//ロード済みフラグを立てる
	gameBgm.volume = title_bgm_volume;//音量を最大にする
	gameBgm.isLoop = true;//ループする
	ChangeVolumeSoundMem(gameBgm.volume, gameBgm.handle);//音量を変更する

	auto& resultBgm = m_sounds[SoundType::ResultBgm];
	resultBgm.handle = loader.GetSound(ResourceLoader::SoundID::ResultBgm);//リソースローダーからリザルトシーンのBGMのハンドルを取得して設定
	resultBgm.loaded = true;//ロード済みフラグを立てる
	resultBgm.volume = title_bgm_volume;//音量を最大にする
	gameBgm.isLoop = true;//ループする
	ChangeVolumeSoundMem(resultBgm.volume, resultBgm.handle);//音量を変更する
}

void SoundManager::Update()
{
	for (auto& [type, data] : m_sounds)
	{
		if (data.fadeState == FadeState::None) continue;//フェードしてないならスキップ

		//フェードの経過時間を更新する
		data.fadeTimer++;

		if (data.fadeState == FadeState::FadeIn)
		{
			//targetVolumeに向かってcurrentVolumeを増やす
			//fadeの進行度割合に応じてcurrentVolumeを更新する
			data.currentVolume = static_cast<int>(data.fadeTimer / data.fadeDuration * data.targetVolume);
			//音量が255を超えないようにする
			data.currentVolume = std::min(data.currentVolume, data.targetVolume);

			//currentVolumeがtargetVolumeに達したらフェードイン完了
			if (data.currentVolume >= data.targetVolume)
			{
				data.currentVolume = data.targetVolume;//currentVolumeをtargetVolumeに合わせる
				//音量を変更する
				data.fadeState = FadeState::None;//フェード状態をNoneにする
			}
			ChangeVolumeSoundMem(data.currentVolume, data.handle);
		}
		else if (data.fadeState == FadeState::FadeOut)
		{
			//targetVolumeに向かってcurrentVolumeを減らす
			data.currentVolume = static_cast<int>(data.volume * (1.0f - data.fadeTimer / data.fadeDuration));
			//音量を変更する
			ChangeVolumeSoundMem(data.currentVolume, data.handle);
			//currentVolumeが0になったらフェードアウト完了
			if (data.currentVolume <= 0)
			{
				data.currentVolume = 0;//currentVolumeを0に合わせる
				//音を停止する
				StopSoundMem(data.handle);
				data.fadeState = FadeState::None;//フェード状態をNoneにする
			}
		}
	}
}

void SoundManager::PlayFadeIn(SoundType soundType, float duration, bool loop)
{
	//ロードされていなかったら以降の処理をスキップ
	auto it = m_sounds.find(soundType);
	if (it == m_sounds.end() || !it->second.loaded) return;

	//サウンドのデータを取得する
	auto& data = it->second;

	//フェードインの設定をする
	data.fadeState = FadeState::FadeIn;//フェード状態をフェードインにする
	data.currentVolume = 0;//現在の音量を0にする
	data.targetVolume = data.volume;//目標の音量をデータの音量にする
	data.fadeTimer = 0.0f;//フェードの経過時間を0にする
	data.fadeDuration = duration;//フェードにかける時間を設定する

	//音を再生する
	ChangeVolumeSoundMem(0, data.handle);//音量を0にする
	PlaySoundMem(
		data.handle,
		loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK,//loopがtrueならループそうじゃないならバック再生
		true);
}

void SoundManager::FadeOut(SoundType soundType, float duration)
{
	//ロードされていなかったら以降の処理をスキップ
	auto it = m_sounds.find(soundType);
	if (it == m_sounds.end() || !it->second.loaded) return;

	//サウンドのデータを取得する
	auto& data = it->second;

	//フェードアウトの設定をする
	data.fadeState = FadeState::FadeOut;//フェード状態をフェードアウトにする
	data.currentVolume = data.volume;//現在の音量をデータの音量にする
	data.targetVolume = 0;//目標の音量を0にする
	data.fadeTimer = 0.0f;//フェードの経過時間を0にする
	data.fadeDuration = duration;//フェードにかける時間を設定する
}

void SoundManager::Play(SoundType soundType, bool loop)
{
	//ロードされていなかったら以降の処理をスキップ
	auto it = m_sounds.find(soundType);
	if (it == m_sounds.end() || !it->second.loaded) return;

	const auto& data = it->second;
	//音を再生する
	PlaySoundMem(
		data.handle,
		loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK,//loopがtrueならループそうじゃないならバック再生
		true);
}

void SoundManager::StopSound(SoundType type)
{
	auto it = m_sounds.find(type);
	if (it == m_sounds.end() || !it->second.loaded) return;

	//指定した音を停止させる
	StopSoundMem(it->second.handle);
}

void SoundManager::StopAll()
{
	for (auto& sound : m_sounds)
	{
		//すべての音を停止する
		StopSoundMem(sound.second.handle);
	}
}

void SoundManager::SetVolume(SoundType type, int volume)
{
	auto it = m_sounds.find(type);
	if (it == m_sounds.end()) return;

	auto& data = it->second;
	//音量を矯正
	data.volume = std::clamp(volume, 0, 255);

	//ちゃんとロードされてたら音量を変更する
	if (data.loaded && data.handle != -1)
	{
		ChangeVolumeSoundMem(data.volume, data.handle);
	}
}
