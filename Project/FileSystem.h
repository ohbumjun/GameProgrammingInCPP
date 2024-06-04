#pragma once

#include <string>

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
};

