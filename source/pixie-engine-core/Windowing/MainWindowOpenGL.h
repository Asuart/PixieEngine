#pragma once
#include "pch.h"
#include "MainWindow.h"

namespace PixieEngine {

class MainWindowOpenGL : public MainWindow {
public:
	MainWindowOpenGL(const std::string& name, glm::ivec2 resolution);
	~MainWindowOpenGL();

	virtual void StartFrame() override;
	virtual void EndFrame() override;

	SDL_GLContext GetOpenGLContext() const;

protected:
	SDL_GLContext m_glContext;
};

}