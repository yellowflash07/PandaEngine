// Vertex shader
#version 410

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
layout (location = 7) in vec4 vTangent;
layout (location = 8) in vec4 vBitangent;

uniform mat4 BoneMatrices[150];
uniform bool useBones;

out vec4 colour;
out vec4 vertexWorldPos;	
out vec4 vertexWorldNormal;
out vec2 texCoord;
out vec4 fragPosLightSpace;
out vec3 worldPos;
out vec4 tangent;
out vec4 bitangent;

out vec4 boneId;
out vec4 boneWeight;

uniform bool isShadowMap;
uniform mat4 lightSpaceMatrix;


void main()
{
	
//	gl_Position = MVP * vec4(finalPos, 1.0);
//	gl_Position = MVP * vertModelPosition;
	//vec4 finalPos = vec4(1.0f);

	if(isShadowMap)
	{
		vec4 finalShadowPos = vec4(vPos.xyz, 1.0);
		if (useBones)
		{
		
			mat4 boneTransform = BoneMatrices[int(vBoneId[0])] * vBoneWeight[0] +
								BoneMatrices[int(vBoneId[1])] * vBoneWeight[1] +
								BoneMatrices[int(vBoneId[2])] * vBoneWeight[2] +
								BoneMatrices[int(vBoneId[3])] * vBoneWeight[3];
			finalShadowPos = boneTransform * vec4(vPos.xyz, 1.0);				
		}
		else
		{
			finalShadowPos = vPos;
		}

		worldPos = (matModel * vec4(finalShadowPos.xyz, 1.0)).xyz;
		gl_Position = lightSpaceMatrix * matModel * vec4(finalShadowPos.xyz, 1.0);
		return;
	}

	vec4 finalPos = vec4(vPos.xyz, 1.0);
	if (useBones)
	{
		
		mat4 boneTransform = BoneMatrices[int(vBoneId[0])] * vBoneWeight[0] +
							BoneMatrices[int(vBoneId[1])] * vBoneWeight[1] +
							BoneMatrices[int(vBoneId[2])] * vBoneWeight[2] +
							BoneMatrices[int(vBoneId[3])] * vBoneWeight[3];
		finalPos = boneTransform * vec4(vPos.xyz, 1.0);				
	}
	else
	{
		finalPos = vPos;
	}

	fragPosLightSpace = lightSpaceMatrix * matModel *  vec4(finalPos.xyz, 1.0);	

	mat4 matMVP = matProjection * matView * matModel;
	gl_Position = matMVP * vec4(finalPos.xyz, 1.0);	
	worldPos = (matModel * vec4(finalPos.xyz, 1.0)).xyz;

	// Rotate the normal by the inverse transpose of the model matrix
	// (so that it only is impacted by the rotation, not translation or scale)
	vertexWorldNormal = matModel_IT * vec4(vNormal.xyz, 1.0f);
	vertexWorldNormal.xyz = normalize(vertexWorldNormal.xyz);
	vertexWorldNormal.w = 1.0f;
	
	vertexWorldPos = matModel * vec4( finalPos.xyz, 1.0f);
	
	colour = vCol;
	texCoord = vTexCoord;
	boneId = vBoneId;
	boneWeight = vBoneWeight;
	tangent = vTangent;
	bitangent = vBitangent;
}
