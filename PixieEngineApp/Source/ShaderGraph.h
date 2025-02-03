#pragma once
#include "pch.h"
#include "BezierCurves.h"
#include "ShaderGraph.h"
#include "MeshGenerator.h"

enum ShaderConnectionType : int32_t {
	undefined = 0,
	int32 = (1 << 0),
	float32 = (1 << 1),
	vec2 = (1 << 2),
	vec3 = (1 << 3),
	vec4 = (1 << 4),
	textureR = (1 << 5),
	textureRGB = (1 << 6),
	// united
	texture = (textureR | textureRGB),
};

class ShaderNode;
struct ShaderNodeConnection;

struct ShaderNodeIn {
	ShaderNode* parent;
	std::string name;
	ShaderConnectionType type;
	ShaderNodeConnection* connection = nullptr;

	ShaderNodeIn(ShaderNode* _parent, const std::string& _name, ShaderConnectionType _type) :
		parent(_parent), name(_name), type(_type) {}
};

struct ShaderNodeOut {
	ShaderNode* parent;
	std::string name;
	ShaderConnectionType type;
	std::vector<ShaderNodeConnection*> connections;
	void* value = nullptr;

	ShaderNodeOut(ShaderNode* _parent, const std::string& _name, void* _value, ShaderConnectionType _type) :
		parent(_parent), name(_name), value(_value), type(_type) {}
};

struct ShaderNodeConnection {
	ShaderNodeOut* from = nullptr;
	ShaderNodeIn* to = nullptr;

	ShaderNodeConnection(ShaderNodeOut* _from, ShaderNodeIn* _to) :
		from(_from), to(_to) {}
};

class ShaderNode {
public:
	ShaderNode(const std::string& name) :
		m_name(name) {};

	virtual void Process() = 0;

	ShaderNodeIn* GetInput(const std::string& name) {
		for (uint32_t i = 0; i < m_inputs.size(); i++) {
			if (m_inputs[i].name == name) {
				return &m_inputs[i];
			}
		}
		return nullptr;
	}

	ShaderNodeOut* GetOutput(const std::string& name) {
		for (uint32_t i = 0; i < m_outputs.size(); i++) {
			if (m_outputs[i].name == name) {
				return &m_outputs[i];
			}
		}
		return nullptr;
	}

	int32_t GetOutputInt(const std::string& name) {
		ShaderNodeOut* out = GetOutput(name);
		if (!out) {
			return 0;
		}
		return *((int32_t*)out->value);
	}

	Float GetOutputFloat(const std::string& name) {
		ShaderNodeOut* out = GetOutput(name);
		if (!out) {
			return 0.0f;
		}
		return *((Float*)out->value);
	}

	Vec2 GetOutputVec2(const std::string& name) {
		ShaderNodeOut* out = GetOutput(name);
		if (!out) {
			return Vec2();
		}
		return *((Vec2*)out->value);
	}

	Vec3 GetOutputVec3(const std::string& name) {
		ShaderNodeOut* out = GetOutput(name);
		if (!out) {
			return Vec3();
		}
		return *((Vec3*)out->value);
	}

	Vec4 GetOutputVec4(const std::string& name) {
		ShaderNodeOut* out = GetOutput(name);
		if (!out) {
			return Vec4();
		}
		return *((Vec4*)out->value);
	}

	GLuint GetOutputTexture(const std::string& name) {
		ShaderNodeOut* out = GetOutput(name);
		if (!out) {
			return 0;
		}
		return *((GLuint*)out->value);
	}

	std::string m_name;
	std::vector<ShaderNodeIn> m_inputs;
	std::vector<ShaderNodeOut> m_outputs;
};

class DefferedRenderNode : public ShaderNode {
public:
	DefferedRenderNode() : 
		ShaderNode("Deffered Render") {
		m_outputs.push_back(ShaderNodeOut(this, "Albedo", &m_albedo, ShaderConnectionType::textureRGB));
		m_outputs.push_back(ShaderNodeOut(this, "Normal", &m_normal, ShaderConnectionType::textureRGB));
		m_outputs.push_back(ShaderNodeOut(this, "Position", &m_position, ShaderConnectionType::textureRGB));
		m_outputs.push_back(ShaderNodeOut(this, "Specular", &m_specular, ShaderConnectionType::textureR));
		m_outputs.push_back(ShaderNodeOut(this, "Metallic", &m_metallic, ShaderConnectionType::textureR));
		m_outputs.push_back(ShaderNodeOut(this, "Roughness", &m_roughness, ShaderConnectionType::textureR));
		m_outputs.push_back(ShaderNodeOut(this, "Depth", &m_depth, ShaderConnectionType::textureR));

		const glm::ivec2 resolution = { 1280, 720 };

		glGenFramebuffers(1, &m_frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

		glGenTextures(1, &m_albedo);
		glBindTexture(GL_TEXTURE_2D, m_albedo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_albedo, 0);

		glGenTextures(1, &m_normal);
		glBindTexture(GL_TEXTURE_2D, m_normal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normal, 0);

		glGenTextures(1, &m_position);
		glBindTexture(GL_TEXTURE_2D, m_position);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_position, 0);

		glGenTextures(1, &m_specular);
		glBindTexture(GL_TEXTURE_2D, m_specular);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, resolution.x, resolution.y, 0, GL_RED, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_specular, 0);

		glGenTextures(1, &m_metallic);
		glBindTexture(GL_TEXTURE_2D, m_metallic);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, resolution.x, resolution.y, 0, GL_RED, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_metallic, 0);

		glGenTextures(1, &m_roughness);
		glBindTexture(GL_TEXTURE_2D, m_roughness);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, resolution.x, resolution.y, 0, GL_RED, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, m_roughness, 0);

		GLuint attachments[6] = { 
			GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5
		};
		glDrawBuffers(6, attachments);

		glGenTextures(1, &m_depth);
		glBindTexture(GL_TEXTURE_2D, m_depth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, resolution.x, resolution.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depth, 0);


		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			throw "Failed to initializa FrameBuffer\n";
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Process() override {
		std::shared_ptr<Scene> scene = SceneManager::GetScene();
		if (!scene) return;
		const std::vector<CameraComponent*>& cameras = scene->GetCameras();
		if (cameras.size() == 0) return;
		Camera* camera = &cameras[0]->GetCamera();

		// Store Original Viewport
		GLint originalViewport[4];
		glGetIntegerv(GL_VIEWPORT, originalViewport);

		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
		glViewport(0, 0, 1280, 720);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_program.Bind();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_albedo);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_normal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_position);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_specular);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, m_metallic);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, m_roughness);

		m_program.SetUniform3f("cameraPos", camera->GetTransform().GetPosition());
		m_program.SetUniformMat4f("mView", camera->GetViewMatrix());
		m_program.SetUniformMat4f("mProjection", camera->GetProjectionMatrix());
		DrawObject(scene->GetRootObject(), Mat4(1.0f));
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Restore original viewport
		glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);
	}

	void DrawObject(SceneObject* object, Mat4 parentTransform) {
		if (!object) return;

		Mat4 objectTransform = parentTransform * object->GetTransform().GetMatrix();
		if (const MeshAnimatorComponent* animatorComponent = object->GetComponent<MeshAnimatorComponent>()) {
			std::array<Mat4, MaxBonesPerModel> boneMatricesBuffer;
			animatorComponent->GetBoneMatrices(0.0f, boneMatricesBuffer);
			m_program.SetUniformMat4fv("finalBonesMatrices", &boneMatricesBuffer[0][0][0], (int32_t)boneMatricesBuffer.size());
		}
		if (const MeshComponent* mesh = object->GetComponent<MeshComponent>()) {
			m_program.SetUniformMat4f("mModel", objectTransform);
			Material* material = ResourceManager::GetDefaultMaterial();
			if (MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>()) {
				material = materialComponent->GetMaterial();
			}
			SetupMaterial(material);
			mesh->Draw();
		}
		for (size_t i = 0; i < object->GetChildren().size(); i++) {
			DrawObject(object->GetChild((int32_t)i), objectTransform);
		}
	}

	void SetupMaterial(Material* material) {
		m_program.SetUniform3f("albedo", material->m_albedo.GetRGB());
		m_program.SetUniform1f("metallic", material->m_metallic);
		m_program.SetUniform1f("roughness", material->m_roughness);
		m_program.SetUniform1i("useDiffuseMap", material->m_albedoTexture != nullptr);
		if (material->m_albedoTexture) {
			m_program.SetTexture("albedoTexture", material->m_albedoTexture->id, 3);
		}
	}

protected:
	Shader m_program;
	GLuint m_frameBuffer = 0;
	GLuint m_albedo = 0;
	GLuint m_normal = 0;
	GLuint m_position = 0;
	GLuint m_specular = 0;
	GLuint m_metallic = 0;
	GLuint m_roughness = 0;
	GLuint m_depth = 0;
};

class WorldSpaceAmbientOcclusionNode : public ShaderNode {
public:
	WorldSpaceAmbientOcclusionNode() :
		ShaderNode("World Space AO") {
		m_inputs.push_back(ShaderNodeIn(this, "Normal", ShaderConnectionType::textureRGB));
		m_inputs.push_back(ShaderNodeIn(this, "Position", ShaderConnectionType::textureRGB));
		m_inputs.push_back(ShaderNodeIn(this, "Depth", ShaderConnectionType::textureR));

		m_outputs.push_back(ShaderNodeOut(this, "AO", &m_ao, ShaderConnectionType::textureR));
	}

	void Process() override {

	}

protected:
	GLuint m_ao;
};

class DefferedShadingNode : public ShaderNode {
public:
	DefferedShadingNode() :
		ShaderNode("Deffered Shading") {

		m_inputs.push_back(ShaderNodeIn(this, "Albedo", ShaderConnectionType::textureRGB));
		m_inputs.push_back(ShaderNodeIn(this, "Normal", ShaderConnectionType::textureRGB));
		m_inputs.push_back(ShaderNodeIn(this, "Position", ShaderConnectionType::textureRGB));
		m_inputs.push_back(ShaderNodeIn(this, "Specular", ShaderConnectionType::textureR));
		m_inputs.push_back(ShaderNodeIn(this, "Metallic", ShaderConnectionType::textureR));
		m_inputs.push_back(ShaderNodeIn(this, "Roughness", ShaderConnectionType::textureR));
		m_inputs.push_back(ShaderNodeIn(this, "AO", ShaderConnectionType::textureR));

		m_outputs.push_back(ShaderNodeOut(this, "Frame", &m_frame, ShaderConnectionType::textureRGB));
	}

	void Process() override {

	}

protected:
	GLuint m_frame;
};

class DisplayTextureNode : public ShaderNode {
public:
	DisplayTextureNode() :
		ShaderNode("Display Texture") {
		m_inputs.push_back(ShaderNodeIn(this, "Texture", ShaderConnectionType::texture));
	}

	void Process() override {

	}
};

class ShaderGraph {
public:
	ShaderGraph() {
		m_nodes.push_back(new DefferedRenderNode());
		m_nodes.push_back(new WorldSpaceAmbientOcclusionNode());
		m_nodes.push_back(new DefferedShadingNode());
		m_nodes.push_back(new DisplayTextureNode());

		Connect(m_nodes[0]->GetOutput("Position"), m_nodes[1]->GetInput("Position"));
		Connect(m_nodes[0]->GetOutput("Normal"), m_nodes[1]->GetInput("Normal"));
		Connect(m_nodes[0]->GetOutput("Depth"), m_nodes[1]->GetInput("Depth"));

		Connect(m_nodes[1]->GetOutput("AO"), m_nodes[2]->GetInput("AO"));


		for (int32_t i = 0; i < m_nodes[0]->m_outputs.size() - 1; i++) {
			Connect(&m_nodes[0]->m_outputs[i], &m_nodes[2]->m_inputs[i]);
		}

		Connect(&m_nodes[2]->m_outputs[0], &m_nodes[3]->m_inputs[0]);
	}

	ShaderNodeConnection* Connect(ShaderNodeOut* from, ShaderNodeIn* to) {
		ShaderNodeConnection* connection = new ShaderNodeConnection(from, to);
		from->connections.push_back(connection);
		if (to->connection) {
			RemoveConnection(to->connection);
		}
		to->connection = connection;
		m_connections.push_back(connection);
		return connection;
	}

	void RemoveConnection(ShaderNodeConnection* connection) {

	}

	std::vector<ShaderNode*> m_nodes;
	std::vector<ShaderNodeConnection*> m_connections;
};
