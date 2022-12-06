#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

class Texture2D
{
public:
    // ID da textura
    unsigned int ID;
    // Dimensões da imagem
    unsigned int Width, Height; // Em pixels
    // Formato da textura
    unsigned int Internal_Format; // Formato do objeto
    unsigned int Image_Format; // Formato da imagem
    // Config
    unsigned int Wrap_S; // Wrapping mode no eixo S
    unsigned int Wrap_T; // wrapping mode no eixo T
    unsigned int Filter_Min; // Filtering mode se pixels < pixels da tela
    unsigned int Filter_Max; // Filtering mode se pixels > pixels da tela
    // Construtor
    Texture2D();
    // Gera textura de imagem
    void Generate(unsigned int width, unsigned int height, unsigned char* data);
    // Bind
    void Bind() const;
};

#endif