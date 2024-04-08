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
	2D 에서의 회전은, unit circle 위에서의 각도와 동일하다
	unit circle 에서 angle 의 공식은
	x = cos 각도, y = sin 각도

	Math::Cos(mRotation)  : 내가 현재 향하는 방향의 x
	Math::Sin(mRotation)) : 내가 현재 향하는 방향의 y
	- 를 해주는 이유는.unit circle 은 y+ 가 up 인데, sdl 2d graphic 에서는 y+ 가 down 이기 때문이다.

	리턴값을 normalize 해줄 필요는 없다.
	왜냐하면, unit circle equation(공식) 은 항상 unit length vector 를 리턴한다.
	왜냐하면 uni circle 의 radius 가 1 이기 때문이다.
	*/
	return Vector2(Math::Cos(mRotation), -Math::Sin(mRotation));
}
