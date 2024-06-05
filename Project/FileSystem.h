#pragma once

#include <string>
#include <algorithm>

class FileSystem
{
public:
	static std::string GetFileExtension(const std::string& filename) {
		// Find the position of the last dot (.)
		size_t pos = filename.rfind('.');

		// If no dot is found, return an empty string
		if (pos == std::string::npos) {
			return "";
		}

		// Extract the extension (everything after the last dot)
		return filename.substr(pos + 1);
	}
	static std::string GetPathName(const char* path)
	{
		std::string result;
		if (nullptr == path)
		{
			throw("path is nullptr.");
			return result;
		}
	
		size_t len = strlen(path);
	
		if (len == 0)
		{
			return path;
		}
	
		const char* p = &path[len - 1];
		const char* s = path;
	
		while (p != s)
		{
			if (*p == '\\' || *p == '/')
			{
				result.append(p + 1, 0, static_cast<int>(len - (p + 1 - s)));
				return result;
			}
	
			if (*p == ':')
			{
				return result;
			}
			p--;
		}
	
		result.append(path);
		return result;
	}

	static std::string CombinePath(const char* a, const char* b);
	
	// static std::string CombinePath(const char* a, const std::string& b);
	
	// static std::string CombinePath(const std::string& a, const char* b);
	
	static std::string CombinePath(const std::string& a, const std::string& b);
};

