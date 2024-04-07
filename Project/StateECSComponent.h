#pragma once

#include "ECSComponent.h"

class StateECSComponent : public ECSComponent
{
public:
	enum State
	{
		EActive,
		EPaused,
		EDead
	};

	StateECSComponent() = default;
	~StateECSComponent() = default;

	State GetState() const { return mState; }
	void SetState(State state) { mState = state; }
private:
	// Actor's state
	State mState = State::EActive;
};

