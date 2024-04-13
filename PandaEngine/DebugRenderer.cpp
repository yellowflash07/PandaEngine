#include "DebugRenderer.h"
#include "Camera.h"
static DebugRenderer* instance = 0;

extern Camera* camera;

DebugRenderer::DebugRenderer()
{
    cShaderManager::cShader vertexShader;
    // vertexShader.fileName = "vsPhong.glsl";
    vertexShader.fileName = "DebugVert.glsl";

    cShaderManager::cShader tessControlShader;
    // tessControlShader.fileName = "tcsTriangle.glsl";
    tessControlShader.fileName = "";

    cShaderManager::cShader tessEvalShader;
    // tessEvalShader.fileName = "tesTriangle.glsl";
    tessEvalShader.fileName = "";

    cShaderManager::cShader fragmentShader;
    // fragmentShader.fileName = "fsPhong.glsl";
    fragmentShader.fileName = "DebugFrag.glsl";

    cShaderManager::getInstance()->createProgramFromFile("DebugShader",
        		vertexShader,
        		fragmentShader,
                tessControlShader,
                tessEvalShader);
    this->shaderProgramID = cShaderManager::getInstance()->getIDFromFriendlyName("DebugShader");
}

DebugRenderer::~DebugRenderer()
{
}

void DebugRenderer::DrawLine(glm::vec3 start, glm::vec3 end, glm::vec3 color0, glm::vec3 color1)
{
    glUseProgram(shaderProgramID);

    GLuint cameraView_location = glGetUniformLocation(shaderProgramID, "cameraView");
    glUniformMatrix4fv(cameraView_location, 1, GL_FALSE, glm::value_ptr(camera->matView));

    if (this->lines.empty())
    {
        DebugLine line(start, end, color0, color1);
		this->lines.push_back(line);
        LoadLineToGPU(line);
    }

    //check if the line already exists
    for (const DebugLine line : this->lines)
    {
        if (line.start == start && line.end == end)
        {
            glBindBuffer(GL_ARRAY_BUFFER, line.vbo);
            glBufferSubData(GL_ARRAY_BUFFER,
                0,  // Offset
                sizeof(sVertex) * 6,
                (GLvoid*)line.vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return;
		}
	}

    DebugLine line(start, end, color0, color1);
	this->lines.push_back(line);
	LoadLineToGPU(line);

   

}

DebugRenderer* DebugRenderer::getInstance()
{
    if (!instance)
    {
	   instance = new DebugRenderer();
    }
    return instance;
}

void DebugRenderer::LoadLineToGPU(DebugLine& line)
{
	glGenVertexArrays(1, &line.vao);
    glBindVertexArray(line.vao);

	glGenBuffers(1, &line.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, line.vbo);

    sVertex* vertices = line.vertices;

    glBufferData(GL_ARRAY_BUFFER, sizeof(sVertex) * 6, (GLvoid*)vertices, GL_STATIC_DRAW);



    GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");	// program;
    GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");	// program

    glEnableVertexAttribArray(vpos_location);	    // vPos
    glVertexAttribPointer(vpos_location, 4,		// vPos
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, x));

    // Color attribute
    glEnableVertexAttribArray(vcol_location);	    // vCol
    glVertexAttribPointer(vcol_location, 4,		// vCol
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, r));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Draw the line
    glBindVertexArray(line.vao);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);

}
