#include "Renderer.h"

SDL_Renderer* Renderer::mRenderer = nullptr;
SDL_Window* Renderer::mWindow = nullptr;

bool Renderer::initialize()
{
	
	// Create SDL renderer
	/*
	Renderer 란 ?
	graphic 을 그리는 system 을 renderer 라고 한다.
	*/
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

	return true;
}

void Renderer::finalize()
{
	SDL_DestroyWindow(mWindow);
	SDL_DestroyRenderer(mRenderer);
}

SDL_Renderer* Renderer::GetRenderer()
{
	return mRenderer;
}

SDL_Window* Renderer::GetWindow()
{
	return mWindow;
}

void Renderer::postRender()
{
	// 끝점 : Front 와 Back Buffer 를 Swap 한다.
	SDL_RenderPresent(mRenderer);
}

void Renderer::preRender()
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

}
