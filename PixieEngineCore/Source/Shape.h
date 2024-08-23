#pragma once
#include "pch.h"
#include "Transform.h"
#include "ShapeSampleContext.h"
#include "ShapeSample.h"

class Shape {
public:
	const Transform& GetTransform() const;
	Float Area();
	std::optional<ShapeSample> Sample(ShapeSampleContext context, Vec2 u);
	Float SamplePDF(ShapeSampleContext context, Vec3 wi);

protected:
	Transform m_transform;
};