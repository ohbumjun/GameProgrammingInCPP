#pragma once

#include "ECSComponent.h"
#include "Math.h"

class CircleECSComponent : public ECSComponent
{
public :
	CircleECSComponent();
	void Update(float deltaTime) override;
	void SetRadius(float radius) { mRadius = radius; }
	float GetRadius() const;

	const Vector2& GetCenter() const;
private:
	float mRadius;
};

