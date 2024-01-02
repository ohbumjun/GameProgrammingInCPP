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
	����.
	- ���� ship �� asteroid �� ���� �ٶ󺸰� �ϰ� �ʹٰ� �غ���.
	1) ���� ship ���� aesteroid ���������� vector �� �����
	�׸��� normalize �Ѵ�
	2) atan2 �� Ȱ���Ͽ� ������ ������� forward vector ��
	���ο� angle �� ������ش�
	3) ����������, ship �� rotation �� �ش� angle �� �������ش�.
	�̶� �߿��� ���� y component �� ������ �༭ SDL 2d ��ǥ�迡
	������� �Ѵ�.


	Vector2 shipToAsteroid = asteroid->GetPosition() - GetPosition();
	shipToAsteroid.Normalize();
	float angle = Math::Atan2(-shipToAsteroid.y, shipToAsteroid.x);
	ship->SetRotation(angle);

	��, �̿� ���� 2D game ������, ��, xy �ุ ����ϴ� 2D Grame ������
	atan2 �� �̿��Ͽ� ������ ���ؾ� �Ѵ�.

	3D ������ ���� Dot Product �� ����ؾ� �Ѵ�.
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