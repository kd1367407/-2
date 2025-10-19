#pragma once
#include"../GameType/GameType.h"
#include"../Src/Framework/Component/ColliderComponent/Shape.h"
#include"../Src/Framework/Component/RigidbodyComponent/RigidbodyComponent.h"

//Modelが管理する純粋なブロックの状態データ
struct BlockState
{
	unsigned int entityId = 0;//GameObjectと紐づけるための一意なID
	BlockType type = BlockType::None;
	Math::Vector3 pos;
	Math::Vector3 rot;
	Math::Vector3 scale;
	std::string renderModelPath;
	std::string archetypeName;
	bool isSwappable;

	//当たり判定用
	Shape::Type shapeType = Shape::Type::Polygon;
	std::string collisionModelPath;
	float radius = 0.5f;
	Math::Vector3 extents = { 0.5,0.5,0.5 };
	Math::Vector3 offset;
	bool isTrigger = false;

	//rigidbody用
	bool hasRigidbody = false;
	RigidbodyType rigidbidyType = RigidbodyType::Static;

	//動くブロック用
	bool isMovingBlock = false;
	Math::Vector3 startPos;
	Math::Vector3 endPos;
	float duration = 2.0f;

	//転移ブロック用
	bool isTransferBlock = false;
	int transferID = 0;//0は無効

	//ジャンプブロック
	bool isJumpBlock = false;
	Math::Vector3 jumpDirection = {};
	float jumpForce;

	//滑るブロック
	bool isSlipperyBlock = false;
	float slipperyDragCoefficient = 1.0f;

	//回転ブロック
	bool isRotatingBlock = false;
	Math::Vector3 rotationAxis = { 0.0,1.0,0.0 };
	float rotationAmount = 90.0f;
	float rotationSpeed = 5.0f;

	//落下ブロック
	bool isSinkingBlock = false;
	Math::Vector3 sinkingInitialPos = {};
	float maxSinkDistance = 2.0f;
	float acceleration = 3.0f;
	float riseSpeed = 1.0f;

	//拡縮ブロック
	bool isScalingBlock = false;
	Math::Vector3 scaleAxis = { 0,1,0 };
	float scaleAmount = 0.5f;
	float scaleSpeed = 1.0f;

	//トリガーブロック(?)
	bool hasTutorialTrigger = false;
	std::string tutorialBlockName;
	std::string tutorialText;
	std::string tutorialImagePath;

};