#pragma once

namespace PixieEngine {

class UIWindow {
public:
	virtual void Draw() = 0;

protected:
	UIWindow() {};
};

}