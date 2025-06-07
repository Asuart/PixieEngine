#include "pch.h"
#include "FileReader.h"

std::string FileReader::ReadFileAsString(const std::filesystem::path& filePath) {
	std::ifstream t(filePath);
	if (!t.is_open()) {
		Log::Error("Failed to open file: %s", filePath.string());
		return "";
	}
	std::string file((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	return file;
}
