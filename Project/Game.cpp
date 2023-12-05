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
	
	// winodw 의 left side
	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f/2.0f;

	// Ball Pos 는 center
	mBallPos.x = 1024.0f/2.0f;
	mBallPos.y = 768.0f/2.0f;

	// 200 pixel per second to left
	mBallVel.x = -200.0f;
	// 200 pixel per second to up
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
	
	// Update paddle position based on direction
	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;

		// 아래 조건들은, screen 범위를 벗어나지 않게 하는 것이다.
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
	
	// Bounce if needed. Did we intrsect with the paddle?
	// Paddle.y 과 BallPos.y 사이의 절대값을 구한다.
	float diff = mPaddlePos.y - mBallPos.y;

	diff = (diff > 0.0f) ? diff : -diff;

	// 첫번째 if 문은 ball 과 paddle 이 충돌한 경우를 고려하는 것이다.
	if (
		// Ball 이 정확한 x 위치에 있을 때
		mBallPos.x <= 25.0f 
		&& mBallPos.x >= 20.0f 
		// y 차이가 충분히 작을 때 -> 만약 diff > paddleH/2.0f 라면
		//							즉, y 값 차이가 paddle 높이의 절반보다 크다면. ball 이 paddle 에 비해
		//							너무 high 에 있거나 너무 낮게 있다는 의미가 된다.
		&& diff <= paddleH/2.0f
		// Ball 이 왼쪽으로 이동
		&& mBallVel.x < 0.0f)
	{
		mBallVel.x *= -1.0f;
		mBallPos.x = 26.0f;
	}
	// Did the ball go off the screen? (if so, end game)
	else if (mBallPos.x <= 0.0f)
	{
		// 왼쪽으로 통과해버린다면 ping pong 이 제대로 안되었다는 의미
		// 이 경우는, 게임을 종료시킨다.
		mIsRunning = false;
	}
	// 오른쪽으로 가고 있고, 오른쪽 벽에 충돌했다면
	else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f)
	{
		mBallVel.x *= -1.0f;
		mBallPos.x = 1024.0f - thickness;
	}
	
	// Did the ball collide with the top wall? 
	// mBallPos.y <= thickness : 하단 벽을 충돌하고
	// mBallVel.y < 0.0f       : 아래로 가고 있다면
	if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
	{
		// 위 아래 속도 및 방향을 다르게 한다.
		mBallVel.y *= -1;

		// 벗어나지 않게 고정시킨다.
		mBallPos.y = thickness;
	}
	// 상단벽을 충돌하고, 상단 벽으로 가고 있다면
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
			// mBallPos 는 Ball 의 center 위치이다.
			// 하지만 SDL_RECT 는 top - left 를 기준으로 그려낸다.
			// center 기준점으로 그리기 위해서, x,y 를 정확하게 반씩 빼줄 것이다.
			static_cast<int>(mBallPos.x - thickness / 2),
			static_cast<int>(mBallPos.y - thickness / 2),
			thickness,
			thickness
		};
		SDL_RenderFillRect(mRenderer, &ball);

	}
	
	// 끝점 : Front 와 Back Buffer 를 Swap 한다.
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}
