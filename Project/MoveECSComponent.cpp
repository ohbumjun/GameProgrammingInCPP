#include "MoveECSComponent.h"
#include "Math.h"
#include "TransformECSComponent.h"

/*
Actor ������ ���� Component �� Update �� ���̴�.
��, MoveComponent �� ��� �ٸ� Component �麸��
���� ���� update �Ǿ�� �ϴ� �༮�� �ϳ��̴�.
*/
void MoveECSComponent::Update(float deltaTime)
{
	TransformECSComponent& transform = mWorld->get_component<TransformECSComponent>(mEntity);

	if (!Math::NearZero(mAngularSpeed))
	{
		// Actor ȸ����Ű�� 
		float rot = transform.GetRotation();
		rot += mAngularSpeed * deltaTime;
		transform.SetRotation(rot);
	}

	if (!Math::NearZero(mForwardSpeed))
	{
		Vector2 pos = transform.GetPosition();

		// ���� �������� �̵� ��Ű�� 
		pos += transform.GetForward() * mForwardSpeed * deltaTime;

		// (Screen wrapping code only for asteroids)
		if (pos.x < 0.0f) { pos.x = 1022.0f; }
		else if (pos.x > 1024.0f) { pos.x = 2.0f; }

		if (pos.y < 0.0f) { pos.y = 766.0f; }
		else if (pos.y > 768.0f) { pos.y = 2.0f; }

		transform.SetPosition(pos);
	}
}
