#pragma once

#include "ECSComponent.h"
#include "Math.h"

class CircleECSComponent : public ECSComponent
{
	CircleECSComponent();

	void SetRadius(float radius) { mRadius = radius; }
	float GetRadius() const;

	const Vector2& GetCenter() const;
private:
	float mRadius;
};

