#pragma once

#include "BaseObject.h"
#include "SDL/SDL_image.h"

class Image : public BaseObject
{
public :
	Image(SDL_Texture* tex) : _tex(tex) {};
private:
	SDL_Texture* _tex = nullptr;
};

