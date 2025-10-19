#include "ParticleSystem.h"
#include"../Src/Framework/Component/GameObject.h"
#include"../../Scene/BaseScene/BaseScene.h"
#include"../Src/Framework/Component/TransformComponent/TransformComponent.h"
#include"../Src/Framework/Component/RenderComponent/RenderComponent.h"
#include"../Src/Framework/Component/TagComponent/TagComponent.h"


void ParticleSystem::Init(BaseScene* pOwnerScene)
{
	//マスターモデルをロード
	m_cubeModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Player/player-1.gltf");
	if (!m_cubeModel)assert("パーティクル用のマスターメッシュが見つかりません");

	//メモリ確保
	m_particles.resize(MAX_PARTICLES);
	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		auto& p = m_particles[i];
		p.gameObject = std::make_shared<GameObject>();
		p.gameObject->SetName("Particle_" + std::to_string(i));
		p.gameObject->AddComponent<TransformComponent>();
		p.gameObject->AddComponent<TagComponent>();
		auto renderComp = p.gameObject->AddComponent<RenderComponent>();
		renderComp->SetModel(m_cubeModel);

		renderComp->SetEnable(false);
		p.lifetime = 0.0f;

		pOwnerScene->AddObject(p.gameObject);
	}
}

void ParticleSystem::Update(float deltatime)
{
	for (auto& p : m_particles)
	{
		if (p.lifetime <= 0.0f)continue;

		p.lifetime -= deltatime;
		p.pos += p.velocity * deltatime;
		p.rot += p.angularVelocity * deltatime;

		if (p.lifetime <= 0.0f)
		{
			p.gameObject->GetComponent<RenderComponent>()->SetEnable(false);
			continue;
		}

		//物理演算
		auto transform = p.gameObject->GetComponent<TransformComponent>();
		if (transform)
		{
			transform->SetPos(p.pos);

			Math::Vector3 rotDeg = {
				DirectX::XMConvertToDegrees(p.rot.x),
				DirectX::XMConvertToDegrees(p.rot.y),
				DirectX::XMConvertToDegrees(p.rot.z)
			};
			transform->SetRot(rotDeg);

			transform->SetScale({ p.size, p.size, p.size });
		}
	}
}

void ParticleSystem::Draw()
{
	KdShaderManager::Instance().m_postProcessShader.BeginBright();
	{
		for (const auto& p : m_particles)
		{
			//アクティブなパーティクルだけを描画
			if (p.lifetime > 0.0f)
			{
				KdShaderManager::Instance().m_StandardShader.DrawModel(
					*m_cubeModel,
					p.gameObject->GetComponent<TransformComponent>()->GetMatrix(),
					p.color
				);
			}
		}
	}
	KdShaderManager::Instance().m_postProcessShader.EndBright();
}

void ParticleSystem::Emit(const Math::Vector3& position, int count, const Math::Color& color, float minSpeed, float maxSpeed, float minLifetime, float maxLifetime, float minSize, float maxSize)
{

	for (int i = 0; i < count; ++i)
	{
		//プールから非アクティブなパーティクルを探す
		Particle* p = nullptr;
		for (auto& particle : m_particles)
		{
			if (particle.lifetime <= 0.0f)
			{
				p = &particle;
				break;
			}
		}

		//空きがなければ終了
		if (!p)break;

		//あればパラメータセット
		p->gameObject->SetName("Particle");
		p->gameObject->GetComponent<RenderComponent>()->SetEnable(true);

		//座標
		p->pos = position;

		//速度(方向含む)
		Math::Vector3 dir = { KdRandom::GetFloat(-1.0f, 1.0f),KdRandom::GetFloat(-1.0f, 1.0f),KdRandom::GetFloat(-1.0f, 1.0f) };
		
		if (dir.LengthSquared() == 0)
		{
			dir.y = 1.0f;
		}
		
		dir.Normalize();
		p->velocity = dir * KdRandom::GetFloat(minSpeed, maxSpeed);

		//色
		Math::Color finalColor = color;
		float colorVariation = 0.3f;//色のバラツキ

		finalColor.R(finalColor.R() + KdRandom::GetFloat(-colorVariation, colorVariation));
		finalColor.G(finalColor.G() + KdRandom::GetFloat(-colorVariation, colorVariation));
		finalColor.B(finalColor.B() + KdRandom::GetFloat(-colorVariation, colorVariation));

		p->color = finalColor;

		//寿命
		p->lifetime = KdRandom::GetFloat(minLifetime, maxLifetime);

		//回転
		p->rot = { KdRandom::GetFloat(-3.14f,3.14),KdRandom::GetFloat(-3.14f,3.14) ,KdRandom::GetFloat(-3.14f,3.14) };
		p->angularVelocity= { KdRandom::GetFloat(-3.14f,3.14),KdRandom::GetFloat(-3.14f,3.14) ,KdRandom::GetFloat(-3.14f,3.14) };
		
		//サイズ
		p->size = KdRandom::GetFloat(minSize, maxSize);
	}
}
