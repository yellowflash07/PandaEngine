#pragma once

#include "../PandaEngine/GraphicsCommon.h"
#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include "TransformComponent.h"
#include "IEditorUI.h"

// This structure matches what's in the shader
class cLight : public IEditorUI
{
public:
    cLight();

    glm::vec4 position;
    glm::vec4 diffuse;	// Colour of the light (used for diffuse)
    glm::vec4 specular;	// rgb = highlight colour, w = power
    glm::vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
    glm::vec4 direction;	// Spot, directional lights
    glm::vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
                    // 0 = pointlight
                    // 1 = spot light
                    // 2 = directional light
    glm::vec4 param2;	// x = 0 for off, 1 for on

    void TurnOn(void);
    void TurnOff(void);

    // These are the uniform locations for this light in the shader
    GLint position_UL;
    GLint diffuse_UL;
    GLint specular_UL;
    GLint atten_UL;
    GLint direction_UL;
    GLint param1_UL;
    GLint param2_UL;

    bool uniformLocationIsSet;

    void SetUniformLocations(GLuint shaderID, int lightIndex);

    void UpdateLight(TransformComponent* transform);

    void Render();
};

class cLightManager
{
public:
    cLightManager();

    // This is called once
    void SetUniformLocations(GLuint shaderID);

    // This is called every frame
    void UpdateUniformValues(GLuint shaderID);

    void UpdateLight(cLight* light, TransformComponent* transform);

    void UpdateLights(GLuint shaderID);

    static const unsigned int NUMBER_OF_LIGHTS_IM_USING = 50;
    cLight* theLights[NUMBER_OF_LIGHTS_IM_USING];
    void DrawBox();
    void LoadLights();
private:
    cLight* selectedLight;
};

