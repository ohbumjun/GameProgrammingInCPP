#pragma once

#include "Uuid.h"

class BaseObject
{
public :
	virtual ~BaseObject() {}

	Uuid GetUuid() const { return uuid; }

protected :
	// uint64 : object handle �� ������ ���� object db ������ ����.
	Uuid uuid;
};

