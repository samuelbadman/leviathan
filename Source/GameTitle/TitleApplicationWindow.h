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
DECLARE_SINGLE_DELEGATE(TitleApplicationWindowDestroyedSignature);

class TitleApplicationWindow : public Core::Window
{
private:
	TitleApplicationWindowResizedSignature ResizedDelegate = {};
	TitleApplicationWindowDestroyedSignature DestroyedDelegate = {};

public:
	TitleApplicationWindow(Core::Engine& Engine, const char* InUniqueName);

	inline TitleApplicationWindowResizedSignature& GetResizedDelegate() { return ResizedDelegate; }
	inline TitleApplicationWindowDestroyedSignature& GetDestroyedDelegate() { return DestroyedDelegate; }

private:
	// Begin window interface
	virtual void OnInputKey(const Core::InputEventArgs& EventArgs) override;
	virtual void OnInputAxis(const Core::InputEventArgs& EventArgs) override;
	virtual void OnResized(uint32_t NewWidth, uint32_t NewHeight) override;
	virtual void OnDestroyed() override;
	// End window interface
};