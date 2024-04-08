#pragma once

#include "ECSComponent.h"
#include "SDL/SDL.h"
#include "Math.h"

class SpriteECSComponent : public ECSComponent
{
public:
	// (Lower draw order corresponds with further back)
	SpriteECSComponent() = default;
	~SpriteECSComponent() = default;

	virtual void Update(float deltaTime) override {};
	virtual void Draw(SDL_Renderer* renderer);
	virtual void SetTexture(SDL_Texture* texture);

	int GetDrawOrder() const { return mDrawOrder; }
	int GetTexHeight() const { return mTexHeight; }
	int GetTexWidth() const { return mTexWidth; }
protected:

	SDL_Texture* mTexture;
	/*
	mDrawOrder 라는 값을 기준으로 순서대로 sprite component 를 그리게 될 것이다.
	*/
	int mDrawOrder;
	int mTexWidth;
	int mTexHeight;
};

