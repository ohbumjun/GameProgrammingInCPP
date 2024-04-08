#pragma once

#include "ECSWorld.h"

class ECSComponent
{
public :
	virtual void Update(float deltaTime) = 0;

	decs::ECSWorld* mWorld;
	decs::EntityID mEntity;
};

