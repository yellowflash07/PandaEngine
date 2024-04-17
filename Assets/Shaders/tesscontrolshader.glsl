#version 420

layout(vertices = 3) out;
uniform vec4 eyeLocation;
in vec3 worldPos[];
in vec4 colour[];
in vec4 vertexWorldPos[];
in vec4 vertexWorldNormal[];
in vec2 texCoord[];
in vec4 fragPosLightSpace[];

out vec3 TworldPos[];
out vec4 Tcolour[];
out vec4 TvertexWorldPos[];
out vec4 TvertexWorldNormal[];
out vec2 TtexCoord[];
out vec4 TfragPosLightSpace[];

uniform float tessLevelOuter;
uniform float tessLevelInner;

uniform bool isLOD;

float GetTessLevel(float Distance0, float Distance1) {
  float AvgDistance = (Distance0 + Distance1) / 2.0;

  AvgDistance = AvgDistance/ 1000.0f;

    if (AvgDistance <= 1.0) {
    return 2.0;
    } else if (AvgDistance <= 2.0) {
    return 1.0;
    } else {
    return 0.6;
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
//        if(isLOD)
//        {
//            float EyeToVertexDistance0 = distance(vec3(eyeLocation), esInWorldPos[0]);
//            float EyeToVertexDistance1 = distance(vec3(eyeLocation), esInWorldPos[1]);
//            float EyeToVertexDistance2 = distance(vec3(eyeLocation), esInWorldPos[2]);
//
//            // Calculate the tessellation levels
//            gl_TessLevelOuter[0] =   GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
//            gl_TessLevelOuter[1] =  GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
//            gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
//            gl_TessLevelInner[0] = gl_TessLevelOuter[2];
//        }
//        else
//		{
//			gl_TessLevelOuter[0] = 1.0f;
//			gl_TessLevelOuter[1] = 1.0f;
//			gl_TessLevelOuter[2] = 1.0f;
//			gl_TessLevelInner[0] = 1.0f;
//		}
        
    }

    Tcolour[gl_InvocationID] = colour[gl_InvocationID];
    TvertexWorldPos[gl_InvocationID] = vertexWorldPos[gl_InvocationID];
    TvertexWorldNormal[gl_InvocationID] = vertexWorldNormal[gl_InvocationID];
    TtexCoord[gl_InvocationID] = texCoord[gl_InvocationID];
    TfragPosLightSpace[gl_InvocationID] = fragPosLightSpace[gl_InvocationID];
    TworldPos[gl_InvocationID] = worldPos[gl_InvocationID];

}
