#include "cVAOManager.h"

#include "../PandaEngine/GraphicsCommon.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

#include <sstream>
#include <fstream>


void cVAOManager::setBasePath(std::string basePathWithoutSlash)
{
    this->m_basePathWithoutSlash = basePathWithoutSlash;
    return;
}

bool cVAOManager::LoadModelIntoVAO(
		std::string fileName, 
		sModelDrawInfo &drawInfo,
	    unsigned int shaderProgramID,
        bool bIsDynamicBuffer /*=false*/)

{
	// Load the model from file

	drawInfo.meshName = fileName;

    std::string fileAndPath = this->m_basePathWithoutSlash + "/" + fileName;

    if ( ! this->m_LoadTheFile_Ply_XYZ_N_RGBA(fileAndPath, drawInfo) )
    {
        return false;
    };

	// TODO: Load the model from file


	// Create a VAO (Vertex Array Object), which will 
	//	keep track of all the 'state' needed to draw 
	//	from this buffer...

	// Ask OpenGL for a new buffer ID...
	glGenVertexArrays( 1, &(drawInfo.VAO_ID) );
	// "Bind" this buffer:
	// - aka "make this the 'current' VAO buffer
	glBindVertexArray(drawInfo.VAO_ID);

	// Now ANY state that is related to vertex or index buffer
	//	and vertex attribute layout, is stored in the 'state' 
	//	of the VAO... 

	glGenBuffers(1, &(drawInfo.VertexBufferID) );

	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);

	glBufferData( GL_ARRAY_BUFFER, 
				  sizeof(sVertex) * drawInfo.numberOfVertices,	
				  (GLvoid*) drawInfo.pVertices,							
				  (bIsDynamicBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW ) );


	// Copy the index buffer into the video card, too
	// Create an index buffer.
	glGenBuffers( 1, &(drawInfo.IndexBufferID) );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

	glBufferData( GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
	              sizeof( unsigned int ) * drawInfo.numberOfIndices, 
	              (GLvoid*) drawInfo.pIndices,
                  GL_STATIC_DRAW );

	// Set the vertex attributes.

	GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");	// program
	GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");	// program;
	GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");	// program;

	// Set the vertex attributes for this shader
	glEnableVertexAttribArray(vpos_location);	    // vPos
	glVertexAttribPointer( vpos_location, 4,		// vPos
						   GL_FLOAT, GL_FALSE,
						   sizeof(sVertex), 
						   ( void* ) offsetof(sVertex, x));

	glEnableVertexAttribArray(vcol_location);	    // vCol
	glVertexAttribPointer( vcol_location, 4,		// vCol
						   GL_FLOAT, GL_FALSE,
                          sizeof(sVertex),
						   ( void* ) offsetof(sVertex, r));

	glEnableVertexAttribArray(vNormal_location);	// vNormal
	glVertexAttribPointer(vNormal_location, 4,		// vNormal
						   GL_FLOAT, GL_FALSE,
                           sizeof(sVertex),
						   ( void* ) offsetof(sVertex, nx));

	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vpos_location);
	glDisableVertexAttribArray(vcol_location);
	glDisableVertexAttribArray(vNormal_location);


	// Store the draw information into the map
	this->m_map_ModelName_to_VAOID[ drawInfo.meshName ] = drawInfo;


	return true;
}


bool cVAOManager::FindDrawInfoByModelName(
		std::string filename,
		sModelDrawInfo &drawInfo) 
{
	std::map< std::string /*model name*/,
			sModelDrawInfo /* info needed to draw*/ >::iterator 
		itDrawInfo = this->m_map_ModelName_to_VAOID.find( filename );

	// Find it? 
	if ( itDrawInfo == this->m_map_ModelName_to_VAOID.end() )
	{
		// Nope
		return false;
	}

	// Else we found the thing to draw
	// ...so 'return' that information
	drawInfo = itDrawInfo->second;
	return true;
}



bool cVAOManager::m_LoadTheFile_Ply_XYZ_N_RGBA(std::string theFileName, sModelDrawInfo& drawInfo)
{

    std::ifstream theFile( theFileName.c_str() );
    if (!theFile.is_open())
    {
        return false;
    }

    std::string temp;
    while (theFile >> temp)
    {
        if (temp == "vertex")
        {
            break;
        }
    };

    theFile >> drawInfo.numberOfVertices;


    while (theFile >> temp)
    {
        if (temp == "face")
        {
            break;
        }
    };

    theFile >> drawInfo.numberOfTriangles;

    drawInfo.numberOfIndices = drawInfo.numberOfTriangles * 3;

    while (theFile >> temp)
    {
        if (temp == "end_header")
        {
            break;
        }
    };


    // This most closely matches the ply file
    struct sVertexPlyFile
    {
        float x;
        float y;
        float z;
        float nx, ny, nz;
        float r, g, b, a;
    };

    struct sTrianglePlyFile
    {
        unsigned int v0, v1, v2;
    };

    sVertexPlyFile* pTheVerticesFile = new sVertexPlyFile[drawInfo.numberOfVertices];


    for (unsigned int index = 0; index != drawInfo.numberOfVertices; index++)
    {
        sVertexPlyFile tempVertex;
        theFile >> tempVertex.x;                //std::cin >> a.x;
        theFile >> tempVertex.y;                //std::cin >> a.y;
        theFile >> tempVertex.z;                //std::cin >> a.z;

        theFile >> tempVertex.nx;
        theFile >> tempVertex.ny;
        theFile >> tempVertex.nz;

        theFile >> tempVertex.r;       tempVertex.r /= 255.0f;
        theFile >> tempVertex.g;       tempVertex.g /= 255.0f;
        theFile >> tempVertex.b;       tempVertex.b /= 255.0f;
        theFile >> tempVertex.a;       tempVertex.a /= 255.0f;


        pTheVerticesFile[index] = tempVertex;
    }


    sTrianglePlyFile* pTheTriangles = new sTrianglePlyFile[drawInfo.numberOfTriangles];

    // 3 3495 3549 3548 
    for (unsigned int index = 0; index != drawInfo.numberOfTriangles; index++)
    {
        sTrianglePlyFile tempTriangle;

        unsigned int discard;
        theFile >> discard;            // 3
        theFile >> tempTriangle.v0;                //std::cin >> a.x;
        theFile >> tempTriangle.v1;                //std::cin >> a.y;
        theFile >> tempTriangle.v2;                //std::cin >> a.z;

        pTheTriangles[index] = tempTriangle;
    }

    // ... now we just copy the vertices from the file as is (unchanged)
    drawInfo.pVertices = new sVertex[drawInfo.numberOfVertices];
    for (unsigned int vertIndex = 0; vertIndex != drawInfo.numberOfVertices; vertIndex++)
    {
        // 3 1582 1581 2063 
        drawInfo.pVertices[vertIndex].x = pTheVerticesFile[vertIndex].x;
        drawInfo.pVertices[vertIndex].y = pTheVerticesFile[vertIndex].y;
        drawInfo.pVertices[vertIndex].z = pTheVerticesFile[vertIndex].z;
        drawInfo.pVertices[vertIndex].w = 1.0f;


        drawInfo.pVertices[vertIndex].nx = pTheVerticesFile[vertIndex].nx;
        drawInfo.pVertices[vertIndex].ny = pTheVerticesFile[vertIndex].ny;
        drawInfo.pVertices[vertIndex].nz = pTheVerticesFile[vertIndex].nz;
        drawInfo.pVertices[vertIndex].nw = 1.0f;

        drawInfo.pVertices[vertIndex].r = pTheVerticesFile[vertIndex].r;
        drawInfo.pVertices[vertIndex].g = pTheVerticesFile[vertIndex].g;
        drawInfo.pVertices[vertIndex].b = pTheVerticesFile[vertIndex].b;
        drawInfo.pVertices[vertIndex].a = pTheVerticesFile[vertIndex].a;
    }

    // Allocate an array for all the indices (which is 3x the number of triangles)
    // Element array is an 1D array of integers
    drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

    unsigned int elementIndex = 0;
    for (unsigned int triIndex = 0; triIndex != drawInfo.numberOfTriangles; triIndex++)
    {
    
        drawInfo.pIndices[elementIndex + 0] = pTheTriangles[triIndex].v0;
        drawInfo.pIndices[elementIndex + 1] = pTheTriangles[triIndex].v1;
        drawInfo.pIndices[elementIndex + 2] = pTheTriangles[triIndex].v2;

        elementIndex += 3;    
    }


    return true;
}



bool cVAOManager::UpdateVAOBuffers(std::string fileName,
                      sModelDrawInfo& updatedDrawInfo,
                      unsigned int shaderProgramID)
{
    // This exists? 
    sModelDrawInfo updatedDrawInfo_TEMP;
    if ( ! this->FindDrawInfoByModelName(fileName,  updatedDrawInfo_TEMP) )
    {
        // Didn't find this buffer
        return false;
    }


    glBindBuffer(GL_ARRAY_BUFFER, updatedDrawInfo.VertexBufferID);

// Original call to create and copy the initial data:
//     
//    glBufferData(GL_ARRAY_BUFFER,
//                 sizeof(sVertex) * updatedDrawInfo.numberOfVertices,	
//                 (GLvoid*)updatedDrawInfo.pVertices,					
//                 GL_DYNAMIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 
                    0,  // Offset
                    sizeof(sVertex) * updatedDrawInfo.numberOfVertices,	
                    (GLvoid*)updatedDrawInfo.pVertices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, updatedDrawInfo.IndexBufferID);

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,			
                    0,  
                    sizeof(unsigned int) * updatedDrawInfo.numberOfIndices,
                    (GLvoid*)updatedDrawInfo.pIndices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    return true;
}
