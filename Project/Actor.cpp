// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include <algorithm>

Actor::Actor(Game* game)
	:mState(EActive)
	, mPosition(Vector2::Zero)
	, mScale(1.0f)
	, mRotation(0.0f)
	, mGame(game)
{
	mGame->AddActor(this);
}

Actor::~Actor()
{
	mGame->RemoveActor(this);
	// Need to delete components
	// Because ~Component calls RemoveComponent, need a different style loop
	while (!mComponents.empty())
	{
		delete mComponents.back();
	}
}

void Actor::Update(float deltaTime)
{
	if (mState == EActive)
	{
		UpdateComponents(deltaTime);
		UpdateActor(deltaTime);
	}
}

void Actor::UpdateComponents(float deltaTime)
{
	for (auto comp : mComponents)
	{
		comp->Update(deltaTime);
	}
}

void Actor::UpdateActor(float deltaTime)
{
}

void Actor::ProcessInput(const uint8_t* keyState)
{
	if (mState == EActive)
	{
		// First process input for components
		for (auto comp : mComponents)
		{
			comp->ProcessInput(keyState);
		}

		ActorInput(keyState);
	}
}

void Actor::ActorInput(const uint8_t* keyState)
{
}

Vector2 Actor::GetForward() const
{ 
	// Convert Angle to Forward Vector
	// 2D 에서의 회전은, unit circle 위에서의 각도와 동일하다
	// unit circle 에서 angle 의 공식은
	// x = cos 각도, y = sin 각도

	// Math::Cos(mRotation)  : 내가 현재 향하는 방향의 x
	// Math::Sin(mRotation)) : 내가 현재 향하는 방향의 y
	// - 를 해주는 이유는.unit circle 은 y+ 가 up 인데, sdl 2d graphic 에서는 y+ 가 down 이기 때문이다.

	// 리턴값을 normalize 해줄 필요는 없다.
	// 왜냐하면, unit circle equation(공식) 은 항상 unit length vector 를 리턴한다.
	// 왜냐하면 uni circle 의 radius 가 1 이기 때문이다.
	return Vector2(Math::Cos(mRotation), -Math::Sin(mRotation)); 
}

void Actor::AddComponent(Component* component)
{
	// Find the insertion point in the sorted vector
	// (The first element with a order higher than me)
	int myOrder = component->GetUpdateOrder();
	auto iter = mComponents.begin();
	for (;
		iter != mComponents.end();
		++iter)
	{
		if (myOrder < (*iter)->GetUpdateOrder())
		{
			break;
		}
	}

	// Inserts element before position of iterator
	mComponents.insert(iter, component);
}

void Actor::RemoveComponent(Component* component)
{
	auto iter = std::find(mComponents.begin(), mComponents.end(), component);
	if (iter != mComponents.end())
	{
		mComponents.erase(iter);
	}
}