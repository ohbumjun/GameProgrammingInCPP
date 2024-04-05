#pragma once

#include "Math.h"
#include "BaseObject.h"
#include "SDL/SDL.h"

struct Sprite : public BaseObject
{
	SDL_Texture* mTexture;
	/*
		mOffset ���� �� ������ update �ؼ� scrolling effect �� �� ���̴�.
		*/
	Vector2 mOffset;
};

