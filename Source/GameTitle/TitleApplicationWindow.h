#pragma once

#include "Core/Window.h"

class TitleApplicationWindow : public Core::Window
{
public:
	TitleApplicationWindow(Core::Engine& Engine, const char* InUniqueName);

private:
	// Begin window interface
	virtual void OnInputEvent(const Core::WindowInputEventArgs& EventArgs) override;
	virtual void OnMaximized() override;
	virtual void OnMinimized() override;
	virtual void OnResized(uint32_t NewWidth, uint32_t NewHeight) override;
	virtual void OnEnterSizeMove() override;
	virtual void OnExitSizeMove() override;
	virtual void OnReceivedFocus() override;
	virtual void OnLostFocus() override;
	virtual void OnCloseSignal() override;
	// End window interface
};