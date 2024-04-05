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
	Renderer �� ?
	graphic �� �׸��� system �� renderer ��� �Ѵ�.
	*/
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		/*
		2 ��° ���� ? � graphic driver �� ����ϰ� �� ���ΰ�
		
		Usually -1 : SDL �� �˾Ƽ� �����ϰ� �Ѵ�.
		*/
		-1,		 

		/*
		3��° ���� : ���� �ɼ�
		SDL_RENDERER_ACCELERATED  : �׷��� �ϵ������ ������ �����
		SDL_RENDERER_PRESENTVSYNC : VSync �� �����ض�
		*/
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
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
	RGB Image �� �����ֱ� ���ؼ���, �� pixel �� ������ �˾ƾ� �Ѵ�.
	CG ���� color buffer �� ��� screnn �� color ������ ������ �̾���
	memory �� �ǹ��Ѵ�.

	display �� ��ũ���� �׷����� ���ؼ� color buffer �� Ȱ���Ѵ�.

	��� color buffer �� 2���� array �̰�, (x,y) index �� screen ������
	pixel �� �����ȴٰ� �����ϸ� �ȴ�.

	�� �����Ӹ��� GenerateOutput() �Լ������� �ش� color buffer ��
	�׷��� ������� write �ϴ� ���̴�.

	>> Color Depth
	Color Buffer �� �޸� ũ��� �� �ȼ��� ǥ���ϱ� ���� bit ���ڿ� �����Ѵ�.
	��, �� �ȼ��� ǥ���ϴ� bit ������ ũ�� Color Buffer ũ�⵵ Ŀ�� ���̰�
	bit ������ ������ Color Buffer ũ�⵵ ���� ���̴�.

	�� pixel �� ǥ���ϴ� bit �� ������ "Color Depth" ��� �Ѵ�.

	���� ���, 24 bit color depth ������ r,g,b �� ���� 8bit �� ����Ѵ�.
	���� game �� alpha ���� ǥ���ϰ� �ʹٸ� 32 bit color depth �� ����ؾ� �Ѵ�.
	�̸� ���� alpha ���� 8 bit �� ǥ���� �� �ִ� ���̴�.

	>> Pixel ���� ǥ�� ���

	8 bit ���� color �� ǥ���ϱ� ���� 2���� ����� �ִ�
	1) interger ���� Ȱ���Ѵ�. 8 bit �� 0 ~ 255 �̱� ������, �ش� ������ ǥ���ϴ�
	�ڷ����� ����Ѵ�.
	2) Ȥ�� 0 ~ 1 ������ ������ Normalize �Ѵ�.

	>> Double Buffering
	2���� Color Buffer �� Ȱ���Ͽ� display ��Ű�� ��
	1���� �����ְ� ���� ��, �ٸ� �ϳ��� �׷��� ������� ����
	������ ������ �ٲ��ִ� ���̴�.

	�׷��� ������ ���� �������� �ִ� color buffer ��
	�� ���ο� ���� ���鼭 ȭ���� ���� ���� �� �ִ�.

	>> vSync
	�׷��� ��� Double Buffering ��ü�����δ� ȭ�� ������ ���� �� ����.
	�ֳ��ϸ� ���� graphic ������� color buffer �� �� ���⵵ ����
	color buffer swapping �� �߻��� �� �ֱ� �����̴�.

	�̿� ���� �ذ�å�� color buffer write �� ���� ������ swap �� ��ٸ���.
	�̷��� ����� vertical synchronization. �� vsync ��� �Ѵ�.

	���� �̿� ���� ��ٸ��� �ð����� ���� frame �� ������ �� �ִ�.
	���� engine �ʿ� vsync �� ������ �� ������ �����ϴ� ��쵵 �ִ�.

	>> adaptive refresh rate
	�������� display �� game ���� display �ڽ��� refresh �� �� �˸��� ����̴�.
	������ �� ��Ŀ����� game �� display ���� ���� refresh ������ �˷��ش�.

	�̸� ���� game �� display �� sync �ȴ�.
	������ �� ����� Ư�� high quality monitor ������ �����ϴ�
	*/

	// Basic Drawing Step
	/*
	1. clear back buffer to color (���� game �� back buffer �� clear)
	2. game scene �� �׸��� (back buffer �� �׸���)
	3. front, back buffer �� �ٲ۴�.
	*/

	// Back Buffer �� Clear �ϱ� ���� ��������
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A
	);

	// ������ : Back Buffer �� Clear �Ѵ�.
	SDL_RenderClear(mRenderer);

	// Draw all sprite components
	for (auto sprite : mSprites)
	{
		sprite->Draw(mRenderer);
	}
	
	// ���� : Front �� Back Buffer �� Swap �Ѵ�.
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
	// ��, SpriteComponent �� DrawOrder �� ���� ������� ������ �׷����� ���ؼ�
	// ������ �����ִ� ���̴�.
	// �̸� ���� GenerateOutput�Լ��� ���� m_Sprites �� ��ȸ�ϸ鼭 �׷����⸸ �ϸ� �ȴ�.
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
		// Image �� load �Ͽ� SDL_Surface �ȿ� ��´�.
		SDL_Surface* surf = IMG_Load(fileName.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", fileName.c_str());
			return nullptr;
		}

		// Create texture from surface (SDL_Surface �� SDL_Texture �� ��ȯ)
		// SDL_Texture �� drawing �� ���� �ʿ��� ���̴�.
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
