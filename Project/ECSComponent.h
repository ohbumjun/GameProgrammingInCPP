#pragma once

#include "ECSWorld.h"

class ECSComponent
{
public :
	virtual void Update(float deltaTime) = 0;
	void Initialize(decs::ECSWorld* world, decs::EntityID Entity);

	decs::ECSWorld* mWorld;
	decs::EntityID mEntity;
};

