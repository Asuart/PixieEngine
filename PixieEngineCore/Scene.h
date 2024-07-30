#ifndef PIXIE_ENGINE_SCENE
#define PIXIE_ENGINE_SCENE

#include "pch.h"
#include "UID.h"
#include "Transform.h"
#include "SceneObject.h"

class Scene {
public:
	const UID id;
	const std::string name;

	Scene(const std::string& name);

	void AddObject(SceneObject* object, SceneObject* parent = nullptr);
	void AddObject(SceneObject* object, const std::string& parentName);
	void RemoveObject(SceneObject* object);
	void RemoveObject(const std::string& objectName);
	void RemoveObjects(const std::string& objectName);
	SceneObject* FindObject(const std::string& objectName);
	std::vector<SceneObject*> FindObjects(const std::string& objectName);
	SceneObject* FindObjectWithComponent(const std::string& componentName);
	std::vector<SceneObject*> FindObjectsWithComponent(const std::string& componentName);

protected:
	virtual void Start();
	virtual void Update();
	virtual void FixedUpdate();

private:
	SceneObject* rootObject;
	std::vector<SceneObject*> flatObjects;
};

#endif // PIXIE_ENGINE_SCENE