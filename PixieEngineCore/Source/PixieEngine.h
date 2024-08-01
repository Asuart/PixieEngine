#ifndef PIXIE_ENGINE
#define PIXIE_ENGINE

#include "UID.h"
#include "Math.h"
#include "Ray.h"
#include "Bounds.h"
#include "Transform.h"
#include "Texture.h"
#include "Material.h"
#include "Vertex.h"
#include "Mesh.h"
#include "OGLMesh.h"
#include "Scene.h"
#include "BSDF.h"
#include "Camera.h"
#include "Shape.h"
#include "PerlinNoise.h"
#include "Primitive.h"
#include "Light.h"
#include "Film.h"
#include "AssimpGLMHelper.h"
#include "RTMaterial.h"
#include "RTScene.h"
#include "ShaderLibrary.h"
#include "CPURayTracer.h"
#include "SceneLoader.h"
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