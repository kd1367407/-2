#pragma once
class GameObject;

//InputHandlerからの入力を受け取る資格を持つクラスが実装すべきインターフェース(約束事)
class IInputReceiver
{
public:
	virtual ~IInputReceiver() = default;

	//選択されたGameObjectを直接受け取る
	virtual void OnBlockSelected(const std::shared_ptr<GameObject>& selectedObject) = 0;

private:

};