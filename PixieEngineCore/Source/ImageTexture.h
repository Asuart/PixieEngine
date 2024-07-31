#pragma once
#include "pch.h"
#include "RTTexture.h"

class ImageTexture : public RTTexture {
public:
	ImageTexture(const std::string& filePath);
	~ImageTexture();
	glm::vec3 Sample(const RTInteraction& intr) const override;

private:
	glm::vec3* data;
	glm::ivec2 resolution;
};
