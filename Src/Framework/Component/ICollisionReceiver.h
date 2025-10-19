#pragma once

class GameObject;

struct CollisionInfo
{
	std::shared_ptr<GameObject> otherObject;//衝突相手
	Math::Vector3 contactPoint;//衝突点
	Math::Vector3 contactNormal;//衝突点の法線
};

class ICollisionReceiver
{
public:
	virtual ~ICollisionReceiver() = default;

	//物理演算システムから衝突情報を通知
	virtual void OnCollision(const CollisionInfo& info) = 0;

private:

};