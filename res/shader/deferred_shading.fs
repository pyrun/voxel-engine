#version 440

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gShadow;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
    float Radius;
};
const int NR_LIGHTS = 32;

uniform Light lights[NR_LIGHTS];

void main()
{  
    // retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    vec3 Shadow = texture(gShadow, TexCoords).rgb;
    
    vec3 lighting  = Diffuse * Shadow;

    // then calculate lighting as usual
    for(int i = 0; i < NR_LIGHTS; ++i)
    {
         // calculate distance between light source and current fragment
        float distance = length(lights[i].Position - FragPos);
        if(distance < lights[i].Radius)
        {
            //lighting = Diffuse; 
            vec3 lightDir = normalize(lights[i].Position - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0) * Diffuse * lights[i].Color;
        
            float attenuation = 5.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);

            diffuse *= attenuation;
            lighting += diffuse;

            /*float attenuation = 50.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
            
            diffuse *= attenuation;
            lighting += diffuse;*/
        }
    }

    FragColor = vec4( lighting, 1.0);
}
