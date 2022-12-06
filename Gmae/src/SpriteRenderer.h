#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture.h"
#include "Shader.h"

#define GLFW_INCLUDE_GLCOREARB
class SpriteRenderer
{
public:
    // Construtor (inicializa shaders/shapes)
    SpriteRenderer(Shader& shader);
    // Destrutor
    ~SpriteRenderer();
    // Renderiza um sprite em um quad
    void DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
private:
    // Render state
    Shader shader;
    unsigned int quadVAO;
    // Inicializa e configura as propriedade e atributos do quad
    void initRenderData();
};

#endif