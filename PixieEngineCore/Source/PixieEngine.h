#ifndef PIXIE_ENGINE
#define PIXIE_ENGINE

#include "UID.h"
#include "Math.h"
#include "Ray.h"
#include "Bounds.h"
#include "Transform.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"
#include "OGLMesh.h"
#include "Scene.h"
#include "BSDF.h"
#include "Camera.h"
#include "TriangleCache.h"
#include "PerlinNoise.h"
#include "Primitive.h"
#include "Light.h"
#include "Film.h"
#include "AssimpGLMHelper.h"
#include "Material.h"
#include "ShaderLibrary.h"
#include "ResourceManager.h"
#include "FrameBuffer.h"

class PixieEngine {
public:
	void Start();
	void Pause();
	void Stop();
	void SetScene(Scene* scene);

private:
	Scene* currentScene;

	void Update();
	void FixedUpdate();
};

#endif // PIXIE_ENGINE