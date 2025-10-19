#pragma once

//ゲーム全体で共通して使う型を定義

enum class BlockType
{
	None,
	Movable,
	Wall,
	Goal,
	Moving,
	Transfer,
	Jump,
	Slippery,
	Rotating,
	Sinking,
	Scaling,
	Checkpoint,
	TutorialTrigger
};