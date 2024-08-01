#include "ImageTexture.h"

ImageTexture::ImageTexture(const std::string& filePath) {
	int32_t components_per_pixel = 3;
	uint8_t* d = nullptr;// = stbi_load(filePath, &resolution.x, &resolution.y, &components_per_pixel, components_per_pixel);
	if (d == nullptr) {
		std::cerr << "ERROR: Could not load texture image file: '" << filePath << "'.\n";
		resolution = glm::ivec2(0);
		return;
	}
	int32_t pixelCount = resolution.x * resolution.y;
	if (pixelCount == 0) {
		std::cerr << "ERROR: zero size image: '" << filePath << "'.\n";
		return;
	}
	data = new Vec3[pixelCount];
	for (int32_t i = 0; i < pixelCount; i++) {
		data[i] = Vec3(d[i * components_per_pixel] / 255.0f, d[i * components_per_pixel] / 255.0f, d[i * components_per_pixel + 2] / 255.0f);
	}
	delete[] d;
}

ImageTexture::~ImageTexture() {
	delete[] data;
}

Vec3 ImageTexture::Sample(const RTInteraction& intr) const {
	if (data == nullptr)
		return Vec3(0, 1, 1);

	Float u = glm::clamp(intr.uv.x, 0.0f, 1.0f);
	Float v = 1.0f - glm::clamp(intr.uv.y, 0.0f, 1.0f);

	int32_t i = static_cast<int32_t>(u * resolution.x);
	int32_t j = static_cast<int32_t>(v * resolution.y);

	if (i >= resolution.x)  i = resolution.x - 1;
	if (j >= resolution.y) j = resolution.y - 1;

	return data[j * resolution.x + i];
}