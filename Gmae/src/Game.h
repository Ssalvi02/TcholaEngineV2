#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GameLvl.h"
#include "BallObj.h"
#include "PowerUp.h"

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

// Dados da colisão
typedef std::tuple<bool, Direction, glm::vec2> Collision;

class Game
{
public:
    // Game state
    GameState state;
    bool keys[1024];
    bool keysProcessed[1024];
    unsigned int width, height;
    unsigned int Lives;
    // Construtor/destrutor
    Game(unsigned int w, unsigned int h);
    ~Game();
    // Inicializa o game state (carrega shaders/textures/levels)
    void Init();
    // Game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    //Levels
    std::vector<GameLevel> Levels;
    unsigned int Level;
    // Colisões
    void DoCollisions();
    //Resets
    void ResetLevel();
    void ResetPlayer();
    //PowerUps
    std::vector<PowerUp> PowerUps;
    void SpawnPowerUps(GameObject& block);
    void UpdatePowerUps(float dt);
};


#endif