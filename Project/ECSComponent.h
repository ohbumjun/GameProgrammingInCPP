#pragma once

#include "ECSWorld.h"

class ECSComponent
{
public :
	decs::ECSWorld* mWorld;
	decs::EntityID mEntity;
};

