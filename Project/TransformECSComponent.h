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
	// radian ������ rotation �� �ȴ�.
	// �̰��� ���ؼ� ���� ���ϴ� �������� actor �� rotate ��ų �� �ִ�.
	float mRotation;
};

