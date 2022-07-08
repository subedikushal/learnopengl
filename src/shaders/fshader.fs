#version 330 core
out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
uniform vec3 viewPos;
struct Material{
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform Material material;
void main()
{
    

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.specular * diff * vec3(texture(material.diffuseMap,TexCoords));

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),
    material.shininess);

    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuseMap,TexCoords));

    vec3 specular = light.specular * spec * vec3(texture(material.specularMap,TexCoords));



    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
};

