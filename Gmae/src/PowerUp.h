#ifndef POWER_UP_H
#define POWER_UP_H
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GameObj.h"


// Tamanho de um PowerUp
const glm::vec2 POWERUP_SIZE(60.0f, 20.0f);
// Velocidade que o PowerUp cai
const glm::vec2 VELOCITY(0.0f, 150.0f);

class PowerUp : public GameObject
{
public:
    std::string type;
    float duration;
    bool activated;
    // Construtor
    PowerUp(std::string type_, glm::vec3 color, float duration_, glm::vec2 position, Texture2D texture)
        : GameObject(position, POWERUP_SIZE, texture, color, VELOCITY), type(type_), duration(duration_), activated() { }
};

#endif