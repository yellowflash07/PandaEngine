// Fragment shader
#version 410

in vec4 colour;
in vec4 vertexWorldPos;			// vertex in "world space"
in vec4 vertexWorldNormal;	
in vec2 texCoord;
in vec4 boneId;
in vec4 boneWeight;
in vec4 fragPosLightSpace;

in vec4 tangent;
in vec4 bitangent;

out vec4 outputColour;		// To the frame buffer (aka screen)


//uniform vec3 directionalLightColour;
// rgb are the rgb of the light colour
//uniform vec4 directionalLight_Direction_power;
// xyz is the normalized direction, w = power (between 0 and 1)

// If true, then passes the colour without calculating lighting
uniform bool bDoNotLight;		// Really a float (0.0 or not zero)
uniform bool hasTexture;
uniform bool hasMask;
uniform bool bIsSkyBox;
uniform bool hasVertexColor;
uniform vec4 color;
uniform vec4 eyeLocation;

uniform bool bUseDebugColour;	// if this is true, then use debugColourRGBA for the colour
uniform vec4 debugColourRGBA;	
uniform float transparency;
uniform sampler2D texture_00;
uniform sampler2D texture_01;
uniform sampler2D texture_02;
uniform sampler2D texture_03;
uniform sampler2D maskTexture;

uniform bool hasRenderTexture;
uniform sampler2D renderTexture;

uniform samplerCube skyBoxCubeMap;
uniform vec2 UV_Offset;
uniform vec2 UV_Tiling;
uniform vec4 textureMixRatio_0_3;

struct sLight
{
	vec4 position;			
	vec4 diffuse;	// Colour of the light (used for diffuse)
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

const int NUMBEROFLIGHTS = 50;
uniform sLight theLights[NUMBEROFLIGHTS];  	// 70 uniforms
//... is really:
//uniform vec4 theLights[0].position;
//uniform vec4 theLights[1].position;
//uniform vec4 theLights[2].position;
// etc...

uniform bool useBones;
vec4 calculateLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular, float shadowFactor );
float rand(vec2 co);

uniform bool isShadowMap;
uniform sampler2D shadowMap;
uniform bool hasNormalMap;
uniform sampler2D normalMap;
float calculateShadowFactor(vec4 fragPosLightSpace)
{
	// Shadow value
	float shadow = 0.0f;
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	if(projCoords.z > 1.0)
	{
	   return 0.0;
	}
	projCoords = projCoords * 0.5 + 0.5;
	float bias = 0.005;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	//shadow = currentDepth > closestDepth ? 1.0 : 0.0;
	//float shadow = 0.0;

	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -2; x <= 2; ++x) {
		for(int y = -2; y <= 2; ++y) {
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			float distance = currentDepth - bias - pcfDepth;
			shadow += distance > 0.0f ? 1.0 - abs(distance) : 0.0f;
		}
	}
	shadow /= pow((-2 * 2 + 1), 2);
	return shadow;
}

vec3 calculateBumpNormal()
{
	vec4 Normal = normalize(vertexWorldNormal);
    vec4 Tangent = normalize(tangent);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent.xyz, Normal.xyz);
    vec3 BumpMapNormal = texture(normalMap,  (texCoord.st * UV_Tiling.xy)).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}



void main()
{
//	gl_FragColor = vec4(color, 1.0);

//	if (useBones)
//	{
//		vec4 finalColor = vec4(0.0, 0.0, 0.0, 1.0);
//		finalColor += boneWeight[0] *  vec4(1.0, 0.0, 0.0, 1.0); // Red for bone 1
//		finalColor += boneWeight[1] *  vec4(0.0, 1.0, 0.0, 1.0); // Green for bone 2
//		finalColor += boneWeight[2] *  vec4(0.0, 0.0, 1.0, 1.0); // Blue for bone 3
//		finalColor += boneWeight[3] *  vec4(1.0, 1.0, 0.0, 1.0); // Yellow for bone 4
//		//vec4 weightColor = vec4(boneWeight.x, boneWeight.y, boneWeight.z, 1.0f);
//		outputColour = finalColor;
//		return;
//	}

 
	//return;

	if(isShadowMap)
	{
		float depthValue = texture(shadowMap, texCoord).r;
		outputColour = vec4(vec3(depthValue), 1.0);
		return;
	}

	if ( bIsSkyBox )
	{
		vec4 skyBoxSampleColour = texture( skyBoxCubeMap, vertexWorldNormal.xyz ).rgba;
		outputColour.rgb = skyBoxSampleColour.rgb;
		outputColour.a = 1.0f;
		return;
	}


	if( hasRenderTexture )
	{
		vec4 renderTextureColor =  texture( renderTexture, texCoord.st ).rgba;
		outputColour.rgb = renderTextureColor.rgb;
		outputColour.a = 1.0f;
		//chromatic aberration
//		vec2 color_Offset = vec2(0.01f, 0.01f);
//		vec2 texCoordRed = texCoord.st + color_Offset;
//		vec2 texCoordBlue = texCoord.st - color_Offset;
//		vec4 red = texture( renderTexture, texCoordRed ).rgba;
//		vec4 blue = texture( renderTexture, texCoordBlue ).rgba;
//		outputColour.r = red.r;
//		outputColour.g = renderTextureColor.g;
//		outputColour.b = blue.b;

		//Grain
//		float grain = rand(texCoord.st);
//		outputColour.rgb = mix(renderTextureColor.rgb, vec3(grain), 0.5f);
//		outputColour.a = 1.0f;

		//bloom
		 
		return;
	}


	vec4 vertexRGBA = colour;
	if(hasVertexColor)
	{
		//vertexRGBA = colour;
	}		

	if(hasTexture)
	{

		vec4 texColour = texture( texture_00, (texCoord.st * UV_Tiling.xy)+ UV_Offset.xy).rgba * textureMixRatio_0_3.x 	
						+ texture( texture_01, (texCoord.st * UV_Tiling.xy) ).rgba * textureMixRatio_0_3.y
						+ texture( texture_02,  (texCoord.st * UV_Tiling.xy) ).rgba * textureMixRatio_0_3.z
						+ texture( texture_03,  (texCoord.st * UV_Tiling.xy) ).rgba * textureMixRatio_0_3.w;
		vertexRGBA = texColour;
	}

	if(hasMask)
	{
		float maskValue = texture( maskTexture, texCoord.st ).r;
		if(maskValue < 0.1f)
		{
			discard;
		}
	}

	if ( bUseDebugColour )
	{	
		vertexRGBA = debugColourRGBA;
	}
	
	if ( bDoNotLight )
	{
		outputColour = vertexRGBA;
		return;
	}
	
	// *************************************
	// Hard code the specular (for now)
	vec4 vertexSpecular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	// xyzw or rgba or stuw
	// RGB is the specular highglight colour (usually white or the colour of the light)
	// 4th value is the specular POWER (STARTS at 1, and goes to 1000000s)
	
	float shadowFactor = calculateShadowFactor(fragPosLightSpace);

	vec3 normal = vertexWorldNormal.xyz;
	if(hasNormalMap)
	{
		normal = calculateBumpNormal();
	}

	vec4 vertexColourLit = calculateLightContrib( vertexRGBA.rgb, normal, 
	                                              vertexWorldPos.xyz, vertexSpecular, shadowFactor );
	// *************************************
			
	outputColour.rgb = vertexColourLit.rgb;
	
	// Real gamma correction is a curve, but we'll Rock-n-Roll it here
	outputColour.rgb *= 1.35f;
	
	outputColour.a = transparency;
}


vec4 calculateLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular, float shadowFactor )
{
	vec3 norm = normalize(vertexNormal);
	
	vec4 finalObjectColour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
	for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
	{	
		// ********************************************************
		// is light "on"
		if ( theLights[index].param2.x == 0.0f )
		{	// it's off
			continue;
		}
		
		// Cast to an int (note with c'tor)
		int intLightType = int(theLights[index].param1.x);
		
		// We will do the directional light here... 
		// (BEFORE the attenuation, since sunlight has no attenuation, really)
		if ( intLightType == DIRECTIONAL_LIGHT_TYPE )		// = 2
		{
			float ambient = theLights[index].atten.x;

			// diffuse lighting
			vec3 normal = normalize(vertexNormal);
			vec3 lightDirection = normalize(vec3(theLights[index].position));
			float diffuse = max(dot(normal, lightDirection), 0.0f);

			// specular lighting
			float specular = 0.0f;
			if (diffuse != 0.0f)
			{
				float specularLight =  theLights[index].atten.y;
				vec3 viewDirection = normalize(vec3(eyeLocation) - vertexWorldPos);
				vec3 halfwayVec = normalize(viewDirection + lightDirection);
				float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), theLights[index].atten.z);
				specular = specAmount * specularLight;
			};
			vec3 lightColor = theLights[index].diffuse.rgb;
			vec3 finalCol =  (diffuse * (1.0f - shadowFactor) + ambient) + specular  * (1.0f - shadowFactor) * lightColor;
			finalObjectColour.rgb = vertexMaterialColour.rgb * finalCol;
			return finalObjectColour;		
		}
		
		// Assume it's a point light 
		// intLightType = 0
		
		// Contribution for this light
		vec3 vLightToVertex = theLights[index].position.xyz - vertexWorldPos.xyz;
		float distanceToLight = length(vLightToVertex);	
		vec3 lightVector = normalize(vLightToVertex);
		float dotProduct = dot(lightVector, vertexNormal.xyz);	 
		
		dotProduct = max( 0.0f, dotProduct );	
		
		vec3 lightDiffuseContrib = dotProduct * theLights[index].diffuse.rgb;
			

		// Specular 
		vec3 lightSpecularContrib = vec3(0.0f);
			
		vec3 reflectVector = reflect( -lightVector, normalize(norm.xyz) );

		// Get eye or view vector
		// The location of the vertex in the world to your eye
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

		// To simplify, we are NOT using the light specular value, just the object�s.
		float objectSpecularPower = vertexSpecular.w; 
		
//		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
//			                   * vertexSpecular.rgb;	//* theLights[lightIndex].Specular.rgb
		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
			                   * theLights[index].specular.rgb;
							   
		// Attenuation
		float attenuation = 1.0f / 
				( theLights[index].atten.x + 										
				  theLights[index].atten.y * distanceToLight +						
				  theLights[index].atten.z * distanceToLight*distanceToLight );  	
				  
		// total light contribution is Diffuse + Specular
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;
		
		
		// But is it a spot light
		if ( intLightType == SPOT_LIGHT_TYPE )		// = 1
		{	
		

			// Yes, it's a spotlight
			// Calcualate light vector (light to vertex, in world)
			vec3 vertexToLight = vertexWorldPos.xyz - theLights[index].position.xyz;

			vertexToLight = normalize(vertexToLight);

			float currentLightRayAngle
					= dot( vertexToLight.xyz, theLights[index].direction.xyz );
					
			currentLightRayAngle = max(0.0f, currentLightRayAngle);

			//vec4 param1;	
			// x = lightType, y = inner angle, z = outer angle, w = TBD

			// Is this inside the cone? 
			float outerConeAngleCos = cos(radians(theLights[index].param1.z));
			float innerConeAngleCos = cos(radians(theLights[index].param1.y));
							
			// Is it completely outside of the spot?
			if ( currentLightRayAngle < outerConeAngleCos )
			{
				// Nope. so it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if ( currentLightRayAngle < innerConeAngleCos )
			{
				// Angle is between the inner and outer cone
				// (this is called the penumbra of the spot light, by the way)
				// 
				// This blends the brightness from full brightness, near the inner cone
				//	to black, near the outter cone
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / 
									  (innerConeAngleCos - outerConeAngleCos);
									  
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}
						
		}// if ( intLightType == 1 )
		
		
					
		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb)
								  + (vertexSpecular.rgb  * lightSpecularContrib.rgb );

	}
	
	finalObjectColour.a = 1.0f;
	
	return finalObjectColour;
}

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}