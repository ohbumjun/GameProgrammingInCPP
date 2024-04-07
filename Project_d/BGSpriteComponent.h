// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SpriteComponent.h"
#include <vector>
#include "Math.h"

/*
TODO
p 147 의 parallax scrolling 적용해보기
*/

/*
배경에 해당하는 image 가 scroll 하는 느낌을 주기 위한 Component 이다.
*/
class BGSpriteComponent : public SpriteComponent
{
public:
	// Set draw order to default to lower (so it's in the background)
	BGSpriteComponent(class Actor* owner, int drawOrder = 10);
	// Update/draw overridden from parent
	void Update(float deltaTime) override;
	void Draw(SDL_Renderer* renderer) override;
	// Set the textures used for the background
	void SetBGTextures(const std::vector<SDL_Texture*>& textures);
	// Get/set screen size and scroll speed
	void SetScreenSize(const Vector2& size) { mScreenSize = size; }
	void SetScrollSpeed(float speed) { mScrollSpeed = speed; }
	float GetScrollSpeed() const { return mScrollSpeed; }
private:
	// Struct to encapsulate each bg image and its offset
	struct BGTexture
	{
		SDL_Texture* mTexture;

		/*
		mOffset 값을 매 프레임 update 해서 scrolling effect 를 줄 것이다.
		*/
		Vector2 mOffset;
	};
	std::vector<BGTexture> mBGTextures;
	Vector2 mScreenSize;
	float mScrollSpeed;
};