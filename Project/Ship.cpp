// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Ship.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "Game.h"
#include "Laser.h"

Ship::Ship(Game* game)
	:Actor(game)
	, mLaserCooldown(0.0f)
{
	// Create a sprite component
	SpriteComponent* sc = new SpriteComponent(this, 150);
	sc->SetTexture(game->GetTexture("Assets/Ship.png"));

	// Create an input component and set keys/speed
	InputComponent* ic = new InputComponent(this);
	ic->SetForwardKey(SDL_SCANCODE_W);
	ic->SetBackKey(SDL_SCANCODE_S);
	ic->SetClockwiseKey(SDL_SCANCODE_A);
	ic->SetCounterClockwiseKey(SDL_SCANCODE_D);
	ic->SetMaxForwardSpeed(300.0f);
	ic->SetMaxAngularSpeed(Math::TwoPi);
}

void Ship::UpdateActor(float deltaTime)
{
	mLaserCooldown -= deltaTime;

	/*
	참고.
	- 만약 ship 이 asteroid 를 향해 바라보게 하고 싶다고 해보자.
	1) 먼저 ship 에서 aesteroid 방향으로의 vector 를 만든다
	그리고 normalize 한다
	2) atan2 를 활용하여 위에서 만들어준 forward vector 를
	새로운 angle 로 만들어준다
	3) 마지막으로, ship 의 rotation 을 해당 angle 로 설정해준다.
	이때 중요한 것은 y component 를 뒤집어 줘서 SDL 2d 좌표계에
	맞춰줘야 한다.


	Vector2 shipToAsteroid = asteroid->GetPosition() - GetPosition();
	shipToAsteroid.Normalize();
	float angle = Math::Atan2(-shipToAsteroid.y, shipToAsteroid.x);
	ship->SetRotation(angle);

	단, 이와 같이 2D game 에서는, 즉, xy 축만 사용하는 2D Grame 에서만
	atan2 를 이용하여 각도를 구해야 한다.

	3D 게임의 경우는 Dot Product 를 사용해야 한다.
	*/
}

void Ship::ActorInput(const uint8_t* keyState)
{
	if (keyState[SDL_SCANCODE_SPACE] && mLaserCooldown <= 0.0f)
	{
		// Create a laser and set its position/rotation to mine
		Laser* laser = new Laser(GetGame());
		laser->SetPosition(GetPosition());
		laser->SetRotation(GetRotation());

		// Reset laser cooldown (half second)
		mLaserCooldown = 0.5f;
	}
}