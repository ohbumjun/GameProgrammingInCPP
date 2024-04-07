#pragma once

#include "Common.h"

class ProjectLoader
{
	friend class Game;

	static void LoadProject(const std::string& relativePath);
};

