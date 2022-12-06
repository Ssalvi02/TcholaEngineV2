/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
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
    // Opções
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
    unsigned int RBO; // RBO é usado para o multisampled color buffer
    unsigned int VAO;
    // Inicializa o quad para a renderização de texturas após o post process 
    void initRenderData();
};

#endif