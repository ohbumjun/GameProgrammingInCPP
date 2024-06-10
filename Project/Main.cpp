// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

// 1) Asset ��Ƽ ������ ����Ʈ ���� ����
// 2) Packet System ����
// 3) Async �� Thread ���� ���� ����
// 4) DD ���� Packet �� ��Ʈ��ũ ���� ����
// 5) OpenSSL Library �ý��� ����
// 6) Packet �� ���� Server �� ��������
// 7) Server �� ���� simple �� DB ������
// 8) Packet ������Ʈ DB �� ������Ű��
// 9) ���� Game ���� UI Ŭ���ϸ� Packet ������ ���� �ý��� �����ϱ� 

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
