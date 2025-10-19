#pragma once

class ICameraComponent
{
public:
	virtual ~ICameraComponent() = default;
	virtual const std::shared_ptr<KdCamera>& GetCamera() const = 0;
	virtual class GameObject* GetOwner() const = 0;
private:

};