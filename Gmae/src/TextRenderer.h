#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "Shader.h"

struct Character {
    unsigned int TextureID; // ID da textura do glyph 
    glm::ivec2 Size;        // Temanho do glyph
    glm::ivec2 Bearing;     // Offset top-left to bottom-left
    unsigned int Advance;   // Offset horizontal 
};

class TextRenderer
{
public:
    // Lista de caractéres pre-compilados 
    std::map<char, Character> Characters;
    // Shader
    Shader TextShader;
    // Construtor
    TextRenderer(unsigned int width, unsigned int height);
    // Pré compila uma lista de caractéres de uma fonte
    void Load(std::string font, unsigned int fontSize);
    // Renderiza uma string 
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));
private:
    // Chatisse
    unsigned int VAO, VBO;
};

#endif 