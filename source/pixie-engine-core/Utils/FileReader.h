#pragma once
#include "pch.h"

namespace PixieEngine {

class FileReader {
public:
	static std::string ReadFileAsString(const std::filesystem::path& filePath);
};

}