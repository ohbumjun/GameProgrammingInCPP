// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"
#include "SDL/SDL_image.h"
#include <algorithm>
#include "Actor.h"
#include "SpriteComponent.h"
#include "Ship.h"
#include "BGSpriteComponent.h"
#include "Asteroid.h"
#include "ECSWorld.h"
#include "Random.h"
#include <chrono>

// ECS
#include "MoveECSComponent.h"
#include "TransformECSComponent.h"
#include "SpriteECSComponent.h"

#define USE_ECS 1

const int entityNum = 10000;
const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
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
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		// Flags (0 for no flags set)
		/*
		SDL_WINDOW_FULLSCREEN			Use full-screen mode

		SDL_WINDOW_FULLSCREEN_DESKTOP	Use full-screen mode at
										the current desktop
										resolution (and ignore
										width/height parameters
										to SDL_CreateWindow)

		SDL_WINDOW_OPENGL				Add support for the
										OpenGL graphics library

		SDL_WINDOW_RESIZABLE			Allow the user to resize
										the window
		*/
		0		
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	// Create SDL renderer
	/*
	Renderer 란 ?
	graphic 을 그리는 system 을 renderer 라고 한다.
	*/
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		/*
		2 번째 인자 ? 어떤 graphic driver 를 사용하게 할 것인가
		
		Usually -1 : SDL 이 알아서 결정하게 한다.
		*/
		-1,		 

		/*
		3번째 인자 : 각종 옵션
		SDL_RENDERER_ACCELERATED  : 그래픽 하드웨어의 장점을 살려라
		SDL_RENDERER_PRESENTVSYNC : VSync 를 적용해라
		*/
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
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
	Random::Init();

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
		UpdateGame();
		GenerateOutput();

		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double, std::milli> elapsed_time = end - start;
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
}

void Game::GenerateOutput()
{
	// Basic 2D Graphic
	/*
	
	
	>> Color Buffer ?
	RGB Image 를 보여주기 위해서는, 각 pixel 의 색상을 알아야 한다.
	CG 에서 color buffer 는 모든 screnn 의 color 정보를 가지고 이쓴ㄴ
	memory 를 의미한다.

	display 는 스크린을 그려내기 위해서 color buffer 를 활용한다.

	사실 color buffer 는 2차원 array 이고, (x,y) index 는 screen 에서의
	pixel 에 대응된다고 생각하면 된다.

	매 프레임마다 GenerateOutput() 함수에서는 해당 color buffer 에
	그래픽 결과물을 write 하는 것이다.

	>> Color Depth
	Color Buffer 의 메모리 크기는 각 픽셀을 표현하기 위한 bit 숫자에 의존한다.
	즉, 각 픽셀을 표현하는 bit 개수가 크면 Color Buffer 크기도 커질 것이고
	bit 개수가 작으면 Color Buffer 크기도 작을 것이다.

	각 pixel 을 표현하는 bit 의 개수를 "Color Depth" 라고 한다.

	예를 들어, 24 bit color depth 에서는 r,g,b 가 각각 8bit 를 사용한다.
	만약 game 이 alpha 까지 표현하고 싶다면 32 bit color depth 를 사용해야 한다.
	이를 통해 alpha 값도 8 bit 로 표현할 수 있는 것이다.

	>> Pixel 색상 표현 방법

	8 bit 값의 color 를 표현하기 위한 2가지 방법이 있다
	1) interger 값을 활용한다. 8 bit 면 0 ~ 255 이기 때문에, 해당 범위를 표현하는
	자료형을 사용한다.
	2) 혹은 0 ~ 1 사이의 값으로 Normalize 한다.

	>> Double Buffering
	2개의 Color Buffer 를 활용하여 display 시키는 것
	1개는 보여주고 있을 때, 다른 하나에 그래픽 결과물을 쓰고
	프레임 끝에서 바꿔주는 것이다.

	그렇지 않으면 현재 보여지고 있는 color buffer 에
	또 새로운 값을 쓰면서 화면이 깨져 보일 수 있다.

	>> vSync
	그러나 사실 Double Buffering 자체만으로는 화면 깨짐을 막을 수 없다.
	왜냐하면 아직 graphic 결과물을 color buffer 에 다 쓰기도 전에
	color buffer swapping 이 발생할 수 있기 때문이다.

	이에 대한 해결책은 color buffer write 이 끝날 때까지 swap 을 기다린다.
	이러한 방식을 vertical synchronization. 즉 vsync 라고 한다.

	물론 이와 같이 기다리는 시간으로 인해 frame 은 떨어질 수 있다.
	따라서 engine 쪽에 vsync 를 적용할 지 말지를 선택하는 경우도 있다.

	>> adaptive refresh rate
	이전에는 display 가 game 에게 display 자신이 refresh 할 때 알리는 방식이다.
	하지만 이 방식에서는 game 이 display 에게 언제 refresh 할지를 알려준다.

	이를 통해 game 와 display 는 sync 된다.
	하지만 이 기능은 특정 high quality monitor 에서만 가능하다
	*/

	// Basic Drawing Step
	/*
	1. clear back buffer to color (현재 game 의 back buffer 를 clear)
	2. game scene 을 그린다 (back buffer 에 그린다)
	3. front, back buffer 를 바꾼다.
	*/

	// Back Buffer 를 Clear 하기 위한 설정값들
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A
	);

	// 시작점 : Back Buffer 를 Clear 한다.
	SDL_RenderClear(mRenderer);

	// Draw all sprite components
	for (auto sprite : mSprites)
	{
		sprite->Draw(mRenderer);
	}
	
	// 끝점 : Front 와 Back Buffer 를 Swap 한다.
	SDL_RenderPresent(mRenderer);
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

	// Destroy textures
	for (auto i : mTextures)
	{
		SDL_DestroyTexture(i.second);
	}
	mTextures.clear();
}

void Game::TestECS()
{
	struct TestComp { int i; };
	{
		// auto et = world.new_entity();
		// world.add_component<TestComp>(et, TestComp{ 1 });
		for (int i = 0; i < entityNum; i++) {
			auto et = world.new_entity();
			TransformECSComponent* tcmp = world.add_component<TransformECSComponent>(et);
			SpriteECSComponent* spCmp = world.add_component<SpriteECSComponent>(et);
			MoveECSComponent* moveCmp = world.add_component<MoveECSComponent>(et);
		}
	}
	
}

void Game::Shutdown()
{
	UnloadData();
	IMG_Quit();
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
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
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for (;
		iter != mSprites.end();
		++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// Inserts element before position of iterator
	mSprites.insert(iter, sprite);
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
	// (We can't swap because it ruins ordering)
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}

SDL_Texture* Game::GetTexture(const std::string& fileName)
{
	SDL_Texture* tex = nullptr;
	// Is the texture already in the map?
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		// Load from file
		// Image 를 load 하여 SDL_Surface 안에 담는다.
		SDL_Surface* surf = IMG_Load(fileName.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", fileName.c_str());
			return nullptr;
		}

		// Create texture from surface (SDL_Surface 를 SDL_Texture 로 변환)
		// SDL_Texture 가 drawing 을 위해 필요한 것이다.
		tex = SDL_CreateTextureFromSurface(mRenderer, surf);
		SDL_FreeSurface(surf);
		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
			return nullptr;
		}

		mTextures.emplace(fileName.c_str(), tex);
	}
	return tex;
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
