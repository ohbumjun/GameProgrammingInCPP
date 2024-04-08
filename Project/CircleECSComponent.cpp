#include "CircleECSComponent.h"
#include "TransformECSComponent.h"

CircleECSComponent::CircleECSComponent() :
    mRadius(0.0f)
{
}

void CircleECSComponent::Update(float deltaTime)
{
}

float CircleECSComponent::GetRadius() const
{
    TransformECSComponent& transform = mWorld->get_component<TransformECSComponent>(mEntity);

    return transform.GetScale() * mRadius;
}

const Vector2& CircleECSComponent::GetCenter() const
{
    TransformECSComponent& transform = mWorld->get_component<TransformECSComponent>(mEntity);

    return transform.GetPosition();
}
