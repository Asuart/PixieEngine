#pragma once
#include "RTTexture.h"

class ImageTexture : public RTTexture {
public:
	ImageTexture(const std::string& filePath);
	~ImageTexture();
	Vec3 Sample(const RTInteraction& intr) const override;

private:
	Vec3* data;
	glm::ivec2 resolution;
};
