#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <glad/glad.h>

#include "Texture.h"
#include "Shader.h"

class ResourceManager
{
public:
    // Guarda os recursos
    static std::map<std::string, Shader> Shaders;
    static std::map<std::string, Texture2D> Textures;
    // Carrega um shader program de um arquivo carregando os shaders vertex, fragment. Se gShaderFile não é null, carrega geometry shader
    static Shader LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);
    // Pega um shader
    static Shader GetShader(std::string name);
    // Carrega e gera textura
    static Texture2D LoadTexture(const char* file, bool alpha, std::string name);
    // Pega textura
    static Texture2D GetTexture(std::string name);
    // Desaloca tudo
    static void Clear();
private:
    ResourceManager() { }
    // Shader
    static Shader    loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);
    // Textura
    static Texture2D loadTextureFromFile(const char* file, bool alpha);
};

#endif