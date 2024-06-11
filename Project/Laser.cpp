// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Laser.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "CircleComponent.h"
#include "Asteroid.h"
#include "AssetManager.h"
#include "Image.h"
#include "ImageAsset.h"
#include "SDL/SDL_image.h"

Laser::Laser(Game* game)
	:Actor(game)
	, mDeathTimer(1.0f)
{
	// Create a sprite component
	SpriteComponent* sc = new SpriteComponent(this);

	Asset* laserAsset = AssetManager::GetAssetByPath("Assets/Laser.png");
	ImageAsset* laserTextureAsset = static_cast<ImageAsset*>(laserAsset);
	Image* laserTexture = static_cast<Image*>(laserTextureAsset->GetData());
	SDL_Texture* laserSDLTexture = laserTexture->GetTexture();

	sc->SetTexture(laserSDLTexture);

	// Create a move component, and set a forward speed
	MoveComponent* mc = new MoveComponent(this);
	mc->SetForwardSpeed(800.0f);

	// Create a circle component (for collision)
	mCircle = new CircleComponent(this);
	mCircle->SetRadius(11.0f);
}

void Laser::UpdateActor(float deltaTime)
{
	// If we run out of time, laser is dead
	mDeathTimer -= deltaTime;
	if (mDeathTimer <= 0.0f)
	{
		SetState(EDead);
	}
	else
	{
		// Do we intersect with an asteroid?
		for (auto ast : GetGame()->GetAsteroids())
		{
			if (Intersect(*mCircle, *(ast->GetCircle())))
			{
				// The first asteroid we intersect with,
				// set ourselves and the asteroid to dead
				SetState(EDead);
				ast->SetState(EDead);
				break;
			}
		}
	}
}