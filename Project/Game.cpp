// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
,mIsRunning(true)
,mPaddleDir(0)
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
	
	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f/2.0f;
	mBallPos.x = 1024.0f/2.0f;
	mBallPos.y = 768.0f/2.0f;
	mBallVel.x = -200.0f;
	mBallVel.y = 235.0f;
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
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
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	
	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir += 1;
	}
}

void Game::UpdateGame()
{
	// Compute delta time
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();
	
	// Update paddle position based on direction
	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
		if (mPaddlePos.y < (paddleH/2.0f + thickness))
		{
			mPaddlePos.y = paddleH/2.0f + thickness;
		}
		else if (mPaddlePos.y > (768.0f - paddleH/2.0f - thickness))
		{
			mPaddlePos.y = 768.0f - paddleH/2.0f - thickness;
		}
	}
	
	// Update ball position based on ball velocity
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;
	
	// Bounce if needed
	// Did we intrsect with the paddle?
	float diff = mPaddlePos.y - mBallPos.y;
	diff = (diff > 0.0f) ? diff : -diff;
	if (mBallPos.x <= 25.0f && mBallPos.x >= 20.0f && diff <= paddleH/2.0f
		&& mBallVel.x < 0.0f)
	{
		mBallVel.x *= -1.0f;
		mBallPos.x = 26.0f;
	}
	// Did the ball go off the screen? (if so, end game)
	else if (mBallPos.x <= 0.0f)
	{
		// mIsRunning = false;
	}
	// Did the ball collide with the right wall?
	else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f)
	{
		mBallVel.x *= -1.0f;
		mBallPos.x = 1024.0f - thickness;
	}
	
	// Did the ball collide with the top wall?
	if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
	{
		mBallVel.y *= -1;
		mBallPos.y = thickness;
	}
	// Did the ball collide with the right wall?
	else if (mBallPos.y >= (768 - thickness) &&
		mBallVel.y > 0.0f)
	{
		mBallVel.y *= -1;
		mBallPos.y = 768 - thickness;
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

	{
		// Draw walls
		SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

		// Draw top wall
		SDL_Rect wall{
			0,			// Top left x
			0,			// Top left y
			1024,		// Width
			thickness	// Height
		};
		SDL_RenderFillRect(mRenderer, &wall);

		// Draw bottom wall
		wall.y = 768 - thickness;
		SDL_RenderFillRect(mRenderer, &wall);

		// Draw right wall
		wall.x = 1024 - thickness;
		wall.y = 0;
		wall.w = thickness;
		wall.h = 1024;
		SDL_RenderFillRect(mRenderer, &wall);

		// Draw paddle
		SDL_Rect paddle{
			static_cast<int>(mPaddlePos.x),
			static_cast<int>(mPaddlePos.y - paddleH / 2),
			thickness,
			static_cast<int>(paddleH)
		};
		SDL_RenderFillRect(mRenderer, &paddle);

		// Draw ball
		SDL_Rect ball{
			static_cast<int>(mBallPos.x - thickness / 2),
			static_cast<int>(mBallPos.y - thickness / 2),
			thickness,
			thickness
		};
		SDL_RenderFillRect(mRenderer, &ball);

	}
	
	// ���� : Front �� Back Buffer �� Swap �Ѵ�.
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}
