#include "FileManager.h"


std::string						FileManager::_projectPath;

void FileManager::Initialize(const char* projectPath)
{
    _projectPath = projectPath;
}

std::string FileManager::ToAbsolutePath(const char* relativePath)
{
	std::string projectPath(_projectPath);
	return FileSystem::CombinePath(projectPath, relativePath);
}
