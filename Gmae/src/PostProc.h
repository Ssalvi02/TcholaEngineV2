#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "SpriteRenderer.h"
#include "Shader.h"

class PostProcessor
{
public:
    Shader PostProcessingShader;
    Texture2D Texture;
    unsigned int Width, Height;
    // Op��es
    bool Confuse, Chaos, Shake;
    // Construtor
    PostProcessor(Shader shader, unsigned int width, unsigned int height);
    // Prepara o frame buffer para os efeitos de post process
    void BeginRender();
    // Termina o frame buffer
    void EndRender();
    // Renderiza o post process
    void Render(float time);
private:
    // Estado de render
    unsigned int MSFBO, FBO; // MSFBO = Multisampled FBO.
    unsigned int RBO; // RBO � usado para o multisampled color buffer
    unsigned int VAO;
    // Inicializa o quad para a renderiza��o de texturas ap�s o post process 
    void initRenderData();
};

#endif