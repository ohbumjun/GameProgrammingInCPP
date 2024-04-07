#pragma once

#include "Math.h"
#include "BaseObject.h"
#include "SDL/SDL.h"

struct Sprite : public BaseObject
{
	SDL_Texture* mTexture;
	/*
		mOffset 값을 매 프레임 update 해서 scrolling effect 를 줄 것이다.
		*/
	Vector2 mOffset;
};

