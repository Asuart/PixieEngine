#pragma once
#include "pch.h"

class FileReader {
public:
	static std::string ReadFileAsString(const std::filesystem::path& filePath);
};
