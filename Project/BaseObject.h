#pragma once

#include "Uuid.h"

class BaseObject
{
public :
	virtual ~BaseObject() {}

	Uuid GetUuid() const { return uuid; }

protected :
	// uint64 : object handle 을 적용할 때는 object db 값으로 지정.
	Uuid uuid;
};

