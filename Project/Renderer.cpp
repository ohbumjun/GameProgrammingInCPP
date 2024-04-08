#include "Renderer.h"

SDL_Renderer* Renderer::mRenderer = nullptr;
SDL_Window* Renderer::mWindow = nullptr;

bool Renderer::initialize()
{
	
	// Create SDL renderer
	/*
	Renderer �� ?
	graphic �� �׸��� system �� renderer ��� �Ѵ�.
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
	// ���� : Front �� Back Buffer �� Swap �Ѵ�.
	SDL_RenderPresent(mRenderer);
}

void Renderer::preRender()
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

}
