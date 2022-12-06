#ifndef BALLOBJECT_H
#define BALLOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GameObj.h"
#include "Texture.h"

class BallObject : public GameObject
{
public:
    // Vars
    float radius;
    bool stuck;
    bool sticky, pass;
    // Construtor
    BallObject();
    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);
    // Movimento da bola + clamp da tela
    glm::vec2 Move(float dt, unsigned int window_width);
    // Restart posição
    void Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif