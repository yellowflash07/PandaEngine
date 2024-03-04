// Vertex shader
#version 420

//uniform mat4 MVP;
uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matModel;			// Later...
uniform mat4 matModel_IT;		// Inverse transpose of the model matrix

//uniform vec3 modelScale;
//uniform vec3 modelOffset;
in vec4 vCol;		// was vec3
in vec4 vPos;		// was vec3
in vec4 vNormal;	// NEW for 2023!
in vec2 vTexCoord;
layout (location = 5) in vec4 vBoneId;
layout (location = 6) in vec4 vBoneWeight;

uniform mat4 BoneMatrices[150];
uniform bool useBones;

out vec4 colour;
out vec4 vertexWorldPos;	
out vec4 vertexWorldNormal;
out vec2 texCoord;

out vec4 boneId;
out vec4 boneWeight;

void main()
{
	
//	gl_Position = MVP * vec4(finalPos, 1.0);
//	gl_Position = MVP * vertModelPosition;
	vec4 finalPos = vec4(1.0f);
	if (useBones)
	{
		for(int i = 0 ; i < 4 ; i++)
		{
			if(vBoneId[i] == -1)
			{
				break;
			}
				
			if(vBoneId[i] >=150) 
			{
				finalPos = vPos;
				break;
			}
			int boneIdx = int(vBoneId[i]);
			vec4 localPosition = BoneMatrices[boneIdx] * vPos;
			finalPos += localPosition  * vBoneWeight[i];
		
		//	vec4 localNormal = finalBonesMatrices[boneIds[i]] * vec4(aNormal,0.0f);
		//	totalNormal += localNormal * weights[i];
		}
	}
	else
	{
		finalPos = vPos;
	}

	mat4 matMVP = matProjection * matView * matModel;
	gl_Position = matMVP * vec4(finalPos.xyz, 1.0);	
		
	// Rotate the normal by the inverse transpose of the model matrix
	// (so that it only is impacted by the rotation, not translation or scale)
	vertexWorldNormal = matModel_IT * vec4(vNormal.xyz, 1.0f);
	vertexWorldNormal.xyz = normalize(vertexWorldNormal.xyz);
	vertexWorldNormal.w = 1.0f;
	
	vertexWorldPos = matModel * vec4( vPos.xyz, 1.0f);
	
	colour = vCol;
	texCoord = vTexCoord;
	boneId = vBoneId;
	boneWeight = vBoneWeight;
}
