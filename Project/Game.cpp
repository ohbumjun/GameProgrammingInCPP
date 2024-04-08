// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"
#include "SDL/SDL_image.h"
#include "AssetManager.h"
#include "ProjectLoader.h"
#include "Renderer.h"

#include "Actor.h"
#include "SpriteComponent.h"
#include "Ship.h"
#include "BGSpriteComponent.h"
#include "Asteroid.h"
#include "ECSWorld.h"
#include "Random.h"
#include <chrono>


#define USE_ECS 1

const int entityNum = 100000;
const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
:mTicksCount(0)
,mIsRunning(true)
, mUpdatingActors(false)
{
	
}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	

	/*
	2D Game �� ����ٰ� �����ϸ� SDL ���� image file �� load �� �� ����ϴ� SDL Image ��
	Initialize �Ͽ� ����Ѵ�.

	���ڷ� ���� ������� flag ���·� ������ �� �ִ�.
	IMG_INIT_JPG : JPEG
	IMG_INIT_PNG : PNG
	IMG_INIT_TIF : TIFF
	*/
	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
		return false;
	}

	Renderer::initialize();

	Random::Init();

	AssetManager::initialize();

	ProjectLoader::LoadProject("");

#ifdef USE_ECS
	TestECS();
#else
	LoadData();
#endif // USE_ECS

	mTicksCount = SDL_GetTicks();

	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		auto start = std::chrono::steady_clock::now();
		ProcessInput();
		auto start_time = std::chrono::high_resolution_clock::now();
		UpdateGame();

		auto end_time = std::chrono::high_resolution_clock::now();
		// Calculate elapsed time in milliseconds
		auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

		GenerateOutput();
		bool h = true;
	}
}

void Game::ProcessInput()
{
	SDL_Event event;

	/*
	SDL �� Event �� �����ϴ� ���

	application window �� ���� � action �� ���� ��, ex) resize, close window ��
	�̸� ǥ���ϴ� ����� �ٷ� event �̴�.
	��, ���� �ٸ� action �� ���� �ٸ� event �� �ȴٴ� ���̴�.
	�׷��� user �� � ����(Ȥ�� input)�� �ϸ�, os �κ��� ���α׷��� event �� �ް�
	�ش� event �� �������� ������ ������ �� �ִ�.

	SDL �� event �� internal queue ���·� �����Ѵ�.
	window action �� ���õ� event �Ӹ� �ƴ϶�, input device �� ���õ� event ��
	���������̴�.

	�� �����Ӹ��� game �� event ������ üũ�ؾ� �ϰ�
	�̸� ���� ProcessingInput �� �� �����Ӹ��� �����ϴ� ���̴�.
	*/

	// Use poll event to figure out if user is trying to quit
	while (SDL_PollEvent(&event))
	{
		// ���⿡ �پ��� type �� ���� �̺�Ʈ�� ó���ϰ� �Ѵ�.
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	// Get state of keyboard
	const Uint8* keyState = SDL_GetKeyboardState(NULL);

	if (keyState[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	// ��� Actor �� ��ȸ�ϸ鼭 input �� ó���Ѵ�.
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		// ���ο��� ���� ���ο� Actor Class �� �����Ѥ��ٸ�
		// mActors �� �ƴ϶�, mPendingActors �� ���ο� Actor ����
		// �߰������ �Ѵ�.
		actor->ProcessInput(keyState);
	}
	mUpdatingActors = false;
}

void Game::UpdateGame()
{
	// Compute delta time, Wait until 16ms has elapsed since last frame
	/*
	SDL_GetTicks : delta time �� ����� �����ִ� �Լ�
					SDL_INIT ���� �帥 �ð��� �����Ѵ�.
	*/
	/*
	Frame Limiting
	- Ư�� delta time �� �� ������ game loop �� ��ٸ��� �Ѵ�.

	���� ���, 60 FPS �� target frame rate ��� �غ���
	���� frame �� 15ms ���� ���������� frame limiting �� 1.6ms �� �� ��ٸ��� �ؼ�
	16.6ms target time �� ���߰� �Ѵ�.

	�Ʒ� �Լ���, ��� ������ ���̿� 16ms �� �ҿ�ǵ��� ������ �Ŵ� ���̴�.
	*/
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

	// Delta time is the difference in ticks from last frame
	// (1000.f �� ������ ���� : converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value (�ִ� delta time ����)
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	// ���� frame ������ SDL_GetTicks() �� ����ϰ�, ���� frame ������ SDL_GetTicks ���� ���Ͽ� delta �� ���Ѵ�.
	mTicksCount = SDL_GetTicks();

#ifdef USE_ECS
	// aestroid
	mWorld.for_each([&](MoveECSComponent& comp) 
	{
		comp.Update(deltaTime);
	});

	// ship
	// mShip->Update(deltaTime);
#else
	// Update all actors
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->Update(deltaTime);
	}
	mUpdatingActors = false;

	// Move any pending actors to mActors
	for (auto pending : mPendingActors)
	{
		mActors.emplace_back(pending);
	}
	mPendingActors.clear();

	// Add any dead actors to a temp vector
	std::vector<Actor*> deadActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == Actor::EDead)
		{
			deadActors.emplace_back(actor);
		}
	}

	// Delete dead actors (which removes them from mActors)
	for (auto actor : deadActors)
	{
		delete actor;
}
#endif
}

void Game::GenerateOutput()
{
	Renderer::preRender();
	
#ifdef USE_ECS
	// aestroid
	SDL_Renderer* renderer = Renderer::GetRenderer();

	mWorld.for_each([&](SpriteECSComponent& comp)
	{
		comp.Draw(renderer);
	});
	
#else
	// Draw all sprite components
	// for (auto sprite : mSprites)
	// {
	// 	sprite->Draw(Renderer::GetRenderer());
	// }

	for (auto spriteInfo : mSprites)
	{
		const std::list<class SpriteComponent*>& sprites = spriteInfo.second;

		for (auto sprite : sprites)
		{
			sprite->Draw(Renderer::GetRenderer());
		}
	}
#endif // USE_ECS

	

	Renderer::postRender();
}

void Game::LoadData()
{
	mShip = new Ship(this);
	mShip->SetPosition(Vector2(512.0f, 384.0f));
	mShip->SetRotation(Math::PiOver2);

	// Create asteroids
	const int numAsteroids = entityNum;
	for (int i = 0; i < numAsteroids; i++)
	{
		new Asteroid(this);
	}
	bool h = true;
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	AssetManager::finalize();
}

void Game::TestECS()
{
	{
		// asetroid
		for (int i = 0; i < entityNum; i++) {
			// �Ƹ������� Aestroid �����ڿ��� �������� ���� �����ؾ� �Ѵ�.
			
			auto et = mWorld.new_entity();

			// TransformECSComponent �� ����ִ� ArcheType ���������.
			TransformECSComponent* tcmp = mWorld.add_component<TransformECSComponent>(et);
			tcmp->Initialize(&mWorld, et);

			// Initialize to random position/orientation
			Vector2 randPos = Random::GetVector(Vector2::Zero,
				Vector2(1024.0f, 768.0f));

			tcmp->SetPosition(randPos);
			tcmp->SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));

			// TransformECSComp + SpriteECSComponent
			SpriteECSComponent* spCmp = mWorld.add_component<SpriteECSComponent>(et);
			spCmp->Initialize(&mWorld, et);
			spCmp->SetDrawOrder(100);
			spCmp->SetTexture(AssetManager::GetTexture("Assets/Asteroid.png"));

			// TransformECSComp + SpriteECSComponent +  CircleECSComponent
			CircleECSComponent* circleCmp = mWorld.add_component<CircleECSComponent>(et);
			circleCmp->Initialize(&mWorld, et);
			circleCmp->SetRadius(40.0f);

			// TransformECSComp + SpriteECSComponent +  CircleECSComponent + MoveECSComponent
			MoveECSComponent* moveCmp = mWorld.add_component<MoveECSComponent>(et);
			moveCmp->Initialize(&mWorld, et);
			moveCmp->SetForwardSpeed(150.0f);
		}

		// ship �� �ϴ� ������ �ʱ� 
	}
	
}

void Game::TestUpdateMoveComponent(MoveECSComponent* comp)
{
}

void Game::Shutdown()
{
	UnloadData();
	IMG_Quit();
	Renderer::finalize();
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	// If we're updating actors, need to add to pending
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor* actor)
{
	// Is it in pending actors?
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	// Is it in actors?
	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

void Game::AddSprite(SpriteComponent* sprite)
{
	// Find the insertion point in the sorted vector
	// (The first element with a higher draw order than me)
	// ��, SpriteComponent �� DrawOrder �� ���� ������� ������ �׷����� ���ؼ�
	// ������ �����ִ� ���̴�.
	// �̸� ���� GenerateOutput�Լ��� ���� m_Sprites �� ��ȸ�ϸ鼭 �׷����⸸ �ϸ� �ȴ�.
	
	// int myDrawOrder = sprite->GetDrawOrder();
	// auto iter = mSprites.begin();
	// for (;
	// 	iter != mSprites.end();
	// 	++iter)
	// {
	// 	if (myDrawOrder < (*iter)->GetDrawOrder())
	// 	{
	// 		break;
	// 	}
	// }
	// 
	// // Inserts element before position of iterator
	// mSprites.insert(iter, sprite);

	int myDrawOrder = sprite->GetDrawOrder();

	if (mSprites.find(myDrawOrder) == mSprites.end())
	{
		mSprites.insert(std::make_pair(myDrawOrder, std::list<SpriteComponent*>()));
	}

	mSprites[myDrawOrder].emplace_back(sprite);
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
	int order = sprite->GetDrawOrder();

	// (We can't swap because it ruins ordering)
	// auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	// mSprites.erase(iter);

	std::list<class SpriteComponent*>& spriteList = mSprites[order];

	auto iter = std::find(spriteList.begin(), spriteList.end(), sprite);

	if (iter != spriteList.end())
	{
  		spriteList.erase(iter);
	}
}

void Game::AddAsteroid(Asteroid* ast)
{
	mAsteroids.emplace_back(ast);
}

void Game::RemoveAsteroid(Asteroid* ast)
{
	auto iter = std::find(mAsteroids.begin(),
		mAsteroids.end(), ast);
	if (iter != mAsteroids.end())
	{
		mAsteroids.erase(iter);
	}
}
