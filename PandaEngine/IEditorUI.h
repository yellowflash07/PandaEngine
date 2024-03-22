#pragma once

#include <imgui.h>

class IEditorUI
{
public:
	IEditorUI() {};
	virtual ~IEditorUI() {};

	virtual void Render() = 0;
};

