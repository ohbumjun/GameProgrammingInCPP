#pragma once

#include "Math.h"

class TransformECSComponent 
{
public :
	TransformECSComponent() = default;
	~TransformECSComponent() = default;

	const Vector2& GetPosition() const { return mPosition; }
	void SetPosition(const Vector2& pos) { mPosition = pos; }

	float GetScale() const { return mScale; }
	void SetScale(float scale) { mScale = scale; }

	float GetRotation() const { return mRotation; }
	void SetRotation(float rotation) { mRotation = rotation; }

	Vector2 GetForward() const;
private :
	// Transform
	Vector2 mPosition;
	float mScale;
	// radian 형태의 rotation 이 된다.
	// 이것을 통해서 내가 원하는 방향으로 actor 를 rotate 시킬 수 있다.
	float mRotation;
};

