#include "BxDF.h"

glm::vec3 BxDF::rho(glm::vec3 wo, std::span<const float> uc, std::span<const glm::vec2> u2) const {
	if (wo.z == 0)
		return {};
	glm::vec3 r(0.0f);
	for (size_t i = 0; i < uc.size(); ++i) {
		std::optional<BSDFSample> bs = Sample_f(wo, uc[i], u2[i]);
		if (bs && bs->pdf > 0)
			r += bs->f * AbsCosTheta(bs->wi) / bs->pdf;
	}
	return r / (float)uc.size();
}
