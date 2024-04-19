#version 420

layout(vertices = 3) out;
uniform vec4 eyeLocation;
in vec3 worldPos[];
in vec4 colour[];
in vec4 vertexWorldPos[];
in vec4 vertexWorldNormal[];
in vec2 texCoord[];
in vec4 fragPosLightSpace[];

in vec4 tangent[];
in vec4 bitangent[];

out vec3 TworldPos[];
out vec4 Tcolour[];
out vec4 TvertexWorldPos[];
out vec4 TvertexWorldNormal[];
out vec2 TtexCoord[];
out vec4 TfragPosLightSpace[];
out vec4 Ttangent[];
out vec4 Tbitangent[];

uniform float tessLevelOuter;
uniform float tessLevelInner;

uniform bool isLOD;

float GetTessLevel(float Distance0, float Distance1) 
{
    if(!isLOD)
	{
		return 1.0;
	}

  float AvgDistance = (Distance0 + Distance1) / 2.0;

  AvgDistance = AvgDistance/ 1000.0f;

    if (AvgDistance <= 1.0) {
        return 10.0;
    } else if (AvgDistance <= 2.0) {
        return 5.0;
    } else {
        return 1.0;
    }
}


void main()
{
    if (gl_InvocationID == 0) 
    {
            float EyeToVertexDistance0 = distance(vec3(eyeLocation), TworldPos[0]);
            float EyeToVertexDistance1 = distance(vec3(eyeLocation), TworldPos[1]);
            float EyeToVertexDistance2 = distance(vec3(eyeLocation), TworldPos[2]);

            // Calculate the tessellation levels
            gl_TessLevelOuter[0] =   GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
            gl_TessLevelOuter[1] =  GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
            gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
            gl_TessLevelInner[0] = gl_TessLevelOuter[2];
        
    }


    Tcolour[gl_InvocationID] = colour[gl_InvocationID];
    TvertexWorldPos[gl_InvocationID] = vertexWorldPos[gl_InvocationID];
    TvertexWorldNormal[gl_InvocationID] = vertexWorldNormal[gl_InvocationID];
    TtexCoord[gl_InvocationID] = texCoord[gl_InvocationID];
    TfragPosLightSpace[gl_InvocationID] = fragPosLightSpace[gl_InvocationID];
    TworldPos[gl_InvocationID] = worldPos[gl_InvocationID];
    Ttangent[gl_InvocationID] = tangent[gl_InvocationID];
    Tbitangent[gl_InvocationID] = bitangent[gl_InvocationID];

}
