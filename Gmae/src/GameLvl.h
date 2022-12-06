#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GameObj.h"
#include "SpriteRenderer.h"
#include "ResMan.h"


class GameLevel
{
public:
    // Tijolos
    std::vector<GameObject> Bricks;
    // Construtor
    GameLevel() { }
    // Carrega o level de um arquivo
    void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);
    // Render level
    void Draw(SpriteRenderer& renderer);
    // Checa se o level foi concluído
    bool IsCompleted();
private:
    // Inicializa level pelos dados das tiles
    void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif