#pragma once

#include "FileSystem.h"

class FileManager
{
public :
	static void Initialize(const char* projectPath);
	static std::string	ToAbsolutePath(const char* relativePath);

private :
	static std::string						_projectPath;
};

