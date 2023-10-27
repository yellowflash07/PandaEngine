#pragma once
#include "cMesh.h"
#include <vector>
#include "cLightManager.h"

class SceneSaver
{
public:
	SceneSaver();
	~SceneSaver();

	void SaveMeshes(std::vector<cMesh*> meshes);
	std::vector<cMesh*> LoadMeshes();

	void SaveLights(cLight* theLights[], int size);
	void LoadLights(cLight* theLights[], int size);
};