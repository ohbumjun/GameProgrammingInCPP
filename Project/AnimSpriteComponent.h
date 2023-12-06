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
class AnimSpriteComponent : public SpriteComponent
{
public:
	AnimSpriteComponent(class Actor* owner, int drawOrder = 100);
	// Update animation every frame (overridden from component)
	void Update(float deltaTime) override;
	// Set the textures used for animation
	void SetAnimTextures(const std::vector<SDL_Texture*>& textures);
	// Set/get the animation FPS
	float GetAnimFPS() const { return mAnimFPS; }
	void SetAnimFPS(float fps) { mAnimFPS = fps; }
private:
	/*
	All textures in the animation
	Sprite 2D Animation 을 진행하기 위해서 여러 Texture 들을 vector 형태로 모아두는 것이다.
	n 번째 animation 이 곧 n idx 의 SDL_Texture 가 된다고 생각하면 된다.

	Animation 변경 속도를 조절할 수도 있다.
	ex) character 가 speed 를 얻으면 animation frame rate 이 빨라져서
	Texture 도 더 빨리 바뀌고 등등의 기능을 수행할 수 있다는 것이다.
	*/
	std::vector<SDL_Texture*> mAnimTextures;

	/*
	Current frame displayed
	- 현재까지의 frame 속도. 이것을 int 로 형변환 하고 idx 개념으로 활용해서
	내가 draw 하고자 하는 texture 를 SetTexture 하는 방식이다.
	*/
	float mCurrFrame;

	/*
	Animation frame rate
	- 
	*/
	float mAnimFPS;
};