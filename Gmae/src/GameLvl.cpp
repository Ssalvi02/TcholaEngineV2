#include "GameLvl.h"

#include <iostream>
#include <fstream>
#include <sstream>


void GameLevel::Load(const char* file, unsigned int levelWidth, unsigned int levelHeight)
{
    // Limpa dados velhos
    this->Bricks.clear();
    // Carrega pelo arquivo
    unsigned int tileCode;
    GameLevel level;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<unsigned int>> tileData;
    if (fstream)
    {
        while (std::getline(fstream, line)) // Lê as linhas
        {
            std::istringstream sstream(line);
            std::vector<unsigned int> row;
            while (sstream >> tileCode) // Lê as palavras separadas por espaço
                row.push_back(tileCode);
            tileData.push_back(row);
        }
        if (tileData.size() > 0)
        {
            this->init(tileData, levelWidth, levelHeight);
        }
    }
}

void GameLevel::Draw(SpriteRenderer& renderer)
{
    for (GameObject& tile : this->Bricks)
        if (!tile.destroyed)
            tile.Draw(renderer);
}

bool GameLevel::IsCompleted()
{
    for (GameObject& tile : this->Bricks)
    {
        if (!tile.isSolid && !tile.destroyed)
        {
            return false;
        }
    }
    return true;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
    // Calcula as dimensões pra tudo ficar show na tela
    unsigned int height = tileData.size();
    unsigned int width = tileData[0].size();
    float unit_width = levelWidth / static_cast<float>(width), unit_height = levelHeight / height;
    // Inicializa os tiles
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            // Checa o tipo do bloco 
            if (tileData[y][x] == 1) // Sólido
            {
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
                obj.isSolid = true;
                this->Bricks.push_back(obj);
            }
            else if (tileData[y][x] > 1)	// Não-sólido
            {
                glm::vec3 color = glm::vec3(1.0f); // Branco
                if (tileData[y][x] == 2)
                {
                    color = glm::vec3(0.302f, 0.651f, 1.0f); // Azulzinho brabo
                }
                else if (tileData[y][x] == 3)
                {
                    color = glm::vec3(0.561f, 0.871f, 0.365f); // Verdinho brabo
                }
                else if (tileData[y][x] == 4)
                {
                    color = glm::vec3(1.0f, 0.894f, 0.471f); // Amarelo brabo
                }
                else if (tileData[y][x] == 5) 
                {
                    color = glm::vec3(0.729f, 0.38f, 0.337f);// Laranja meio vermeio brabo tamem
                }
                else if (tileData[y][x] == 6)
                {
                    color = glm::vec3(0.949f, 0.651f, 0.369f);// Laranja brabo 
                }
                else if (tileData[y][x] == 7)
                {
                    color = glm::vec3(1.0f, 0.42f, 0.592f);// Rosinha brabo 
                }
                else if (tileData[y][x] == 8)
                {
                    color = glm::vec3(0.812f, 1.0f, 0.439f);// Lima brabo 
                }
                else if (tileData[y][x] == 9)
                {
                    color = glm::vec3(1.0f, 0.71f, 0.71f);// Rosinha claro brabo 
                }
                else if (tileData[y][x] == 10)
                {
                    color = glm::vec3(0.729f, 0.38f, 0.337f);// Marrom brabo 
                }

                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("block"), color));
            }
        }
    }
}