#include "TransformECSComponent.h"

TransformECSComponent::TransformECSComponent() :
	mPosition(Vector2::Zero)
	, mScale(1.0f)
	, mRotation(0.0f)
{
}

Vector2 TransformECSComponent::GetForward() const
{
	// Convert Angle to Forward Vector
	/*
	2D ������ ȸ����, unit circle �������� ������ �����ϴ�
	unit circle ���� angle �� ������
	x = cos ����, y = sin ����

	Math::Cos(mRotation)  : ���� ���� ���ϴ� ������ x
	Math::Sin(mRotation)) : ���� ���� ���ϴ� ������ y
	- �� ���ִ� ������.unit circle �� y+ �� up �ε�, sdl 2d graphic ������ y+ �� down �̱� �����̴�.

	���ϰ��� normalize ���� �ʿ�� ����.
	�ֳ��ϸ�, unit circle equation(����) �� �׻� unit length vector �� �����Ѵ�.
	�ֳ��ϸ� uni circle �� radius �� 1 �̱� �����̴�.
	*/
	return Vector2(Math::Cos(mRotation), -Math::Sin(mRotation));
}
