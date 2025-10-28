#include "UIHelpers.h"

Math::Rectangle CalculateRectFromWorld(const Math::Vector2& vPos, const Math::Vector2& vSize, float screenWidth, float screenHeight)
{
	float centerScreenX = vPos.x + (screenWidth / 2.0f);
	float centerScreenY = (screenHeight / 2.0f) - vPos.y;

	long rectX = static_cast<long>(centerScreenX - (vSize.x / 2.0f));
	long rectY = static_cast<long>(centerScreenY - (vSize.y / 2.0f));

	return DirectX::SimpleMath::Rectangle(rectX, rectY, static_cast<long>(vSize.x), static_cast<long>(vSize.y));
}
