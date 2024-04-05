#include "SpriteECSComponent.h"
#include "TransformECSComponent.h"

void SpriteECSComponent::Draw(SDL_Renderer* renderer)
{
	 TransformECSComponent& transform = mWorld->get_component<TransformECSComponent>(mEntity);

	if (mTexture)
	{
		SDL_Rect r;

		// Scale the width/height by owner's scale
		r.w = static_cast<int>(mTexWidth * transform.GetScale());
		r.h = static_cast<int>(mTexHeight * transform.GetScale());

		// SDL_RECT 의 위치는 top - left 가 되기 때문에, 가운데 위치로 맞춰준다.
		// Center the rectangle around the position of the owner
		r.x = static_cast<int>(transform.GetPosition().x - r.w / 2);
		r.y = static_cast<int>(transform.GetPosition().y - r.h / 2);

		/*
		SDL 에서 image 를 그리는 방식은 2가지 이다.
		1) SDL_RenderCopy
		2) SDL_RenderCopyEx -> sprite 를 rotate 시키는 것이 가능하다
		*/

		// Draw (have to convert angle from radians to degrees, and clockwise to counter)
		SDL_RenderCopyEx(
			renderer, // render target to draw
			mTexture, // texture to draw
			nullptr,  // source rectangle
			&r,		  // destination rectangle
			-Math::ToDegrees(transform.GetRotation()),  // radian -> degree
			nullptr,  // point of rotation
			SDL_FLIP_NONE	// flip behavior
		);
	}
}

void SpriteECSComponent::SetTexture(SDL_Texture* texture)
{
	mTexture = texture;

	// Set width/height
	// SDL_QueryTexture : texture 의 width, height 를 얻어오는 함수
	SDL_QueryTexture(texture, nullptr, nullptr, &mTexWidth, &mTexHeight);
}
