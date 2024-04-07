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
	Sprite 2D Animation �� �����ϱ� ���ؼ� ���� Texture ���� vector ���·� ��Ƶδ� ���̴�.
	n ��° animation �� �� n idx �� SDL_Texture �� �ȴٰ� �����ϸ� �ȴ�.

	Animation ���� �ӵ��� ������ ���� �ִ�.
	ex) character �� speed �� ������ animation frame rate �� ��������
	Texture �� �� ���� �ٲ�� ����� ����� ������ �� �ִٴ� ���̴�.
	*/
	std::vector<SDL_Texture*> mAnimTextures;

	/*
	Current frame displayed
	- ��������� frame �ӵ�. �̰��� int �� ����ȯ �ϰ� idx �������� Ȱ���ؼ�
	���� draw �ϰ��� �ϴ� texture �� SetTexture �ϴ� ����̴�.
	*/
	float mCurrFrame;

	/*
	Animation frame rate
	- 
	*/
	float mAnimFPS;
};