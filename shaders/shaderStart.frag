#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;
in vec4 fPosEyeLight;

out vec4 fColor;

uniform float fogDensity;

//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

uniform float constant;
uniform float linear;
uniform float quadratic;
uniform vec3 positionPointLight;
uniform vec3 positionPointLight2;
vec3 fireColor = vec3(0.882f, 0.345f, 0.133f);
vec3 blueColor = vec3(-0.2f,0.4f,1.0f);

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;
float shadow=0.0f;

float computeFog()
{
float fragmentDistance=length(fPosEye);
float fogFactor =exp(-pow(fragmentDistance*fogDensity,2));

return clamp(fogFactor, 0.0f, 1.0f);
}

void ComputePointLight(){
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(positionPointLight - fPosEyeLight.xyz);
	float diff = max(dot(normalEye,lightDirN),1.0);
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEyeLight.xyz);
	vec3 reflection = reflect(lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	
	float distance =length(positionPointLight - fPosEyeLight.xyz);
	float attenuation = 25.0 / (constant + linear * distance + 
  			     quadratic * (distance * distance));
				 
	vec3 ambient1 = attenuation*fireColor * texture(diffuseTexture, fTexCoords).rgb;
	vec3 diffuse1 = diff *fireColor* attenuation * texture(diffuseTexture, fTexCoords).rgb;
	vec3 specular1 =specCoeff *fireColor * attenuation * texture(specularTexture, fTexCoords).rgb;
	ambient += ambient1;
	diffuse += diffuse1;
	specular += specular1;


	cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	normalEye = normalize(fNormal);	
	
	//compute light direction
	lightDirN = normalize(positionPointLight2 - fPosEyeLight.xyz);
	diff = max(dot(normalEye,lightDirN),1.0);
	//compute view direction 
	viewDirN = normalize(cameraPosEye - fPosEyeLight.xyz);
	reflection = reflect(lightDirN, normalEye);
	specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	
	distance =length(positionPointLight2 - fPosEyeLight.xyz);
	attenuation = 25.0 / (constant + linear * distance + 
  			     quadratic * (distance * distance));
				 
	ambient1 = attenuation*blueColor * texture(diffuseTexture, fTexCoords).rgb;
	diffuse1 = diff *blueColor* attenuation * texture(diffuseTexture, fTexCoords).rgb;
	specular1 =specCoeff *blueColor * attenuation * texture(specularTexture, fTexCoords).rgb;
	ambient += ambient1;
	diffuse += diffuse1;
	specular += specular1;
}

float computeShadow()
{
// perform perspective divide
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// Transform to [0,1] range
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	
	if (normalizedCoords.z > 1.0f)
		return 0.0f;
	
	// Get closest depth value from light's perspective
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	// Get depth of current fragment from light's perspective
	float currentDepth = normalizedCoords.z;
	
	// Check whether current frag pos is in shadow
	float bias = max(0.05f * (1.0f - dot(fNormal, lightDir)), 0.005f);
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
	
	return shadow;
	
}

void ComputeLight()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}

void main() 
{	
	ambient=vec3(0.0f);
	diffuse=vec3(0.0f);
	specular=vec3(0.0f);
	
	ComputeLight();
	
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;
	ComputePointLight();



	shadow = computeShadow();
	vec3 color = min((ambient +(1.0f - shadow)*diffuse) +(1.0f - shadow)*specular, 1.0f);
	vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);

	fColor = vec4(color, 1.0f);
        if(colorFromTexture.a < 0.1)
        discard;
        fColor = colorFromTexture;

	vec4 newColor = vec4(color, 1.0f);
  
	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	fColor = mix(fogColor, newColor, fogFactor);
    
    
}
