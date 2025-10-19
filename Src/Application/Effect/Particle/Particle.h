#pragma once

class GameObject;

struct Particle
{
	std::shared_ptr<GameObject> gameObject = nullptr;
	Math::Vector3 pos;
	Math::Vector3 velocity;
	Math::Color color;
	float lifetime = 0.0f;
	float size;
	Math::Vector3 rot = Math::Vector3::Zero;
	Math::Vector3 angularVelocity;//どれだけ回転するか
};