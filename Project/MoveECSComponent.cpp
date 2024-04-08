#include "MoveECSComponent.h"
#include "Math.h"
#include "TransformECSComponent.h"

/*
Actor 내부의 여러 Component 를 Update 할 것이다.
단, MoveComponent 의 경우 다른 Component 들보다
가장 먼저 update 되어야 하는 녀석중 하나이다.
*/
void MoveECSComponent::Update(float deltaTime)
{
	TransformECSComponent& transform = mWorld->get_component<TransformECSComponent>(mEntity);

	if (!Math::NearZero(mAngularSpeed))
	{
		// Actor 회전시키기 
		float rot = transform.GetRotation();
		rot += mAngularSpeed * deltaTime;
		transform.SetRotation(rot);
	}

	if (!Math::NearZero(mForwardSpeed))
	{
		Vector2 pos = transform.GetPosition();

		// 전방 방향으로 이동 시키기 
		pos += transform.GetForward() * mForwardSpeed * deltaTime;

		// (Screen wrapping code only for asteroids)
		if (pos.x < 0.0f) { pos.x = 1022.0f; }
		else if (pos.x > 1024.0f) { pos.x = 2.0f; }

		if (pos.y < 0.0f) { pos.y = 766.0f; }
		else if (pos.y > 768.0f) { pos.y = 2.0f; }

		transform.SetPosition(pos);
	}
}
