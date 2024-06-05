#pragma once

#include "BaseObject.h"
#include "SDL/SDL_image.h"

class Image : public BaseObject
{
private:
	SDL_Texture* tex = nullptr;
};

