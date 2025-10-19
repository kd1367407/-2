#pragma once
#include"Particle.h"

class BaseScene;

class ParticleSystem
{
public:
	void Init(BaseScene* pOwnerScene);
	void Update(float deltatime);
	void Draw();

	//指定の場所でパーティクル発生
	void Emit(
		const Math::Vector3& position,       // 発生場所
		int count,                           // 発生個数
		const Math::Color& color,            // パーティクルの色
		float minSpeed, float maxSpeed,      // 飛び散る速さの範囲
		float minLifetime, float maxLifetime,  // 寿命の範囲
		float minSize, float maxSize          // サイズの範囲
	);

private:
	std::vector<Particle> m_particles;
	std::shared_ptr<KdModelData> m_cubeModel;
	static const int MAX_PARTICLES = 200;
};