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
	2D Game 만 만든다고 가정하면 SDL 에서 image file 을 load 할 때 사용하는 SDL Image 를
	Initialize 하여 사용한다.

	인자로 각종 포멧들을 flag 형태로 제공할 수 있다.
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
	SDL 이 Event 를 관리하는 방법

	application window 에 대해 어떤 action 을 취할 때, ex) resize, close window 등
	이를 표현하는 방법이 바로 event 이다.
	즉, 서로 다른 action 은 서로 다른 event 가 된다는 것이다.
	그래서 user 가 어떤 행위(혹은 input)를 하면, os 로부터 프로그램은 event 를 받고
	해당 event 에 반응할지 말지를 결정할 수 있다.

	SDL 은 event 를 internal queue 형태로 보관한다.
	window action 과 관련된 event 뿐만 아니라, input device 와 관련된 event 도
	마찬가지이다.

	매 프레임마다 game 은 event 정보를 체크해야 하고
	이를 위해 ProcessingInput 를 매 프레임마다 실행하는 것이다.
	*/

	// Use poll event to figure out if user is trying to quit
	while (SDL_PollEvent(&event))
	{
		// 여기에 다양한 type 에 따라 이벤트를 처리하게 한다.
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

	// 모든 Actor 를 순회하면서 input 을 처리한다.
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		// 내부에서 만약 새로운 Actor Class 를 생성한ㄴ다면
		// mActors 가 아니라, mPendingActors 에 새로운 Actor 들을
		// 추가해줘야 한다.
		actor->ProcessInput(keyState);
	}
	mUpdatingActors = false;
}

void Game::UpdateGame()
{
	// Compute delta time, Wait until 16ms has elapsed since last frame
	/*
	SDL_GetTicks : delta time 을 계산을 도와주는 함수
					SDL_INIT 이후 흐른 시간을 리턴한다.
	*/
	/*
	Frame Limiting
	- 특정 delta time 이 될 때까지 game loop 을 기다리게 한다.

	예를 들어, 60 FPS 이 target frame rate 라고 해보자
	만약 frame 이 15ms 에서 끝나버리면 frame limiting 은 1.6ms 를 더 기다리게 해서
	16.6ms target time 을 맞추게 한다.

	아래 함수는, 적어도 프레임 사이에 16ms 가 소요되도록 제한을 거는 것이다.
	*/
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

	// Delta time is the difference in ticks from last frame
	// (1000.f 로 나누는 이유 : converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value (최대 delta time 제한)
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	// 이전 frame 에서의 SDL_GetTicks() 를 계산하고, 다음 frame 에서의 SDL_GetTicks 값과 비교하여 delta 를 구한다.
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
			// 아리에서는 Aestroid 생성자에서 세팅해준 일을 진행해야 한다.
			
			auto et = mWorld.new_entity();

			// TransformECSComponent 만 들어있는 ArcheType 만들어진다.
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

		// ship 은 일단 만들지 않기 
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
	// 즉, SpriteComponent 의 DrawOrder 에 맞춰 순서대로 대상들을 그려내기 위해서
	// 정렬을 시켜주는 것이다.
	// 이를 통해 GenerateOutput함수는 그저 m_Sprites 를 순회하면서 그려내기만 하면 된다.
	
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
