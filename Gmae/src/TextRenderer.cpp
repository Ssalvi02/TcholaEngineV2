#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "TextRenderer.h"
#include "ResMan.h"


TextRenderer::TextRenderer(unsigned int width, unsigned int height)
{
    // Carrega e configura o shader
    this->TextShader = ResourceManager::LoadShader("src/Shaders/text.vs", "src/Shaders/text.frag", nullptr, "text");
    this->TextShader.SetMatrix4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f), true);
    this->TextShader.SetInteger("text", 0);
    // Configura o VAO/VBO para os quads de texturas
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::Load(std::string font, unsigned int fontSize)
{
    // Limpa os caractéres
    this->Characters.clear();
    // Inicializa e carrega o FreeType library
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) 
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    // Carrega a fonte como face
    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    // Seta o tamanho para carregar os glyphs 
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    // Desativa a restrição de byte-alignment 
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Então para os primeiros 128 ASCII, compila os caractéres e stora eles
    for (GLubyte c = 0; c < 128; c++)
    {
        // Carrega os caractéres de glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Gera a textura
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Seta a textura 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Stora os caractéres para utilizar depois 
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroi FreeType 
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextRenderer::RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
    // Ativa render state correspondente
    this->TextShader.Use();
    this->TextShader.SetVector3f("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);

    // Itera pelos caractéres
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // Atualiza VBO para cada caractére
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f }
        };
        // Renderiza a textura do glyph em um quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Faz o update de memória do VBO 
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Renderiza quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Percorre os glyphs
        x += (ch.Advance >> 6) * scale; 
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}