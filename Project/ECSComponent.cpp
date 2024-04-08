#include "ECSComponent.h"

void ECSComponent::Initialize(decs::ECSWorld* world, decs::EntityID Entity) 
{
	mWorld = world;
	mEntity = Entity;
}