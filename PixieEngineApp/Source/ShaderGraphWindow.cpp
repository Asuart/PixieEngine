#include "pch.h"
#include "ShaderGraphWindow.h"

void ShaderNodeConnectionObject::UpdateMesh() {
	if (!from || !to) return;
	Vec2 p0 = from->parent->position + from->offset + Vec2(0, 4);
	Vec2 p1 = p0 + Vec2(50, 0);
	Vec2 p3 = to->parent->position + to->offset + Vec2(4, 4);
	Vec2 p2 = p3 - Vec2(50, 0);
	if (mesh) delete mesh;
	mesh = MeshGenerator::BezierMesh(BezierCurve2D(p0, p1, p2, p3), 4.0f, 32);
	mesh->Upload();
}

void ShaderNodeConnectionObject::UpdateTempMesh(Vec2 end) {
	Vec2 p0 = from->parent->position + from->offset + Vec2(0, 4);
	Vec2 p1 = p0 + Vec2(50, 0);
	Vec2 p3 = end;
	Vec2 p2 = p3 - Vec2(50, 0);
	if (mesh) delete mesh;
	mesh = MeshGenerator::BezierMesh(BezierCurve2D(p0, p1, p2, p3), 4.0f, 32);
	mesh->Upload();
}
