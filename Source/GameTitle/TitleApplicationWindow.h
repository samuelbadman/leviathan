#pragma once

#include "Core/Window.h"
#include "Core/ProgrammingTools/Delegate.h"

class TitleApplicationWindow;

struct TitleApplicationWindowResizedParameters
{
	TitleApplicationWindow* pWindow = nullptr;
	uint32_t NewWidth = 0;
	uint32_t NewHeight = 0;
};

DECLARE_SINGLE_DELEGATE_OneParam(TitleApplicationWindowResizedSignature, const TitleApplicationWindowResizedParameters& /* Parameters */);

class TitleApplicationWindow : public Core::Window
{
private:
	TitleApplicationWindowResizedSignature ResizedDelegate = {};

public:
	TitleApplicationWindow(Core::Engine& Engine, const char* InUniqueName);

	inline TitleApplicationWindowResizedSignature& GetResizedDelegate() { return ResizedDelegate; }

private:
	// Begin window interface
	virtual void OnInputKey(const Core::InputEventArgs& EventArgs) override;
	virtual void OnInputAxis(const Core::InputEventArgs& EventArgs) override;
	virtual void OnResized(uint32_t NewWidth, uint32_t NewHeight) override;
	// End window interface
};