
#version 420

layout(triangles, equal_spacing, ccw) in;

in vec4 Tcolour[];
in vec4 TvertexWorldPos[];
in vec4 TvertexWorldNormal[];
in vec2 TtexCoord[];
in vec4 TfragPosLightSpace[];
in vec3 esInWorldPos[];

out vec4 colour;
out vec4 vertexWorldPos;
out vec4 vertexWorldNormal;
out vec2 texCoord;
out vec4 fragPosLightSpace;
out vec3 worldPos;

uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matModel;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2) {
  return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 +
         vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2) {
  return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 +
         vec3(gl_TessCoord.z) * v2;
}

vec4 interpolate4D(vec4 v0, vec4 v1, vec4 v2) {
  return vec4(gl_TessCoord.x) * v0 + vec4(gl_TessCoord.y) * v1 +
		 vec4(gl_TessCoord.z) * v2;
}

uniform bool isShadowMap;
uniform mat4 lightSpaceMatrix;
uniform bool useBones;
uniform mat4 BoneMatrices[150];

void main() {

     worldPos = interpolate3D(esInWorldPos[0], esInWorldPos[1], esInWorldPos[2]);
	if(isShadowMap)
	{
	//	worldPos = (matModel * vec4(worldPos.xyz, 1.0)).xyz;
		gl_Position = lightSpaceMatrix * vec4(worldPos.xyz, 1.0);
		return;
	}   


    mat4 matMVP = matProjection * matView;// * matModel;

    gl_Position = matMVP * vec4(worldPos, 1.0);

    colour = interpolate4D(Tcolour[0], Tcolour[1], Tcolour[2]);
    vertexWorldPos = interpolate4D(TvertexWorldPos[0], TvertexWorldPos[1], TvertexWorldPos[2]);
    vertexWorldNormal = interpolate4D(TvertexWorldNormal[0], TvertexWorldNormal[1], TvertexWorldNormal[2]);
    texCoord = interpolate2D(TtexCoord[0], TtexCoord[1], TtexCoord[2]);
    fragPosLightSpace = interpolate4D(TfragPosLightSpace[0], TfragPosLightSpace[1], TfragPosLightSpace[2]);
}