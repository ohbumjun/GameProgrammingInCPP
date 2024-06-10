// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

// 1) Asset 멀티 쓰레드 임포트 구조 구현
// 2) Packet System 구현
// 3) Async 등 Thread 관련 세팅 설정
// 4) DD 에서 Packet 및 네트워크 구조 구현
// 5) OpenSSL Library 시스템 구현
// 6) Packet 을 실제 Server 에 날려보기
// 7) Server 에 대해 simple 한 DB 만들어보기
// 8) Packet 프로젝트 DB 와 연동시키기
// 9) 실제 Game 에서 UI 클릭하면 Packet 날리는 등의 시스템 구현하기 

int main(int argc, char** argv)
{
	Game game;
	bool success = game.Initialize(argv[0]);
	if (success)
	{
		game.RunLoop();
	}
	game.Shutdown();
	return 0;
}
