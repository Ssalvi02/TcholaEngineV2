#include "BallObj.h"


BallObject::BallObject()
    : GameObject(), radius(12.5f), stuck(true), sticky(false), pass(false) { }

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite)
    : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, glm::vec3(1.0f), velocity), radius(radius), stuck(true), sticky(false), pass(false) { }

glm::vec2 BallObject::Move(float dt, unsigned int window_width)
{
    // Se n?o estiver preso no pad do player
    if (!this->stuck)
    {
        // Move
        this->position += this->velocity * dt;
        // Checa se t? fora, inverte a velocidade e seta pra posi??o certa
        if (this->position.x <= 0.0f)
        {
            this->velocity.x = -this->velocity.x;
            this->position.x = 0.0f;
        }
        else if (this->position.x + this->size.x >= window_width)
        {
            this->velocity.x = -this->velocity.x;
            this->position.x = window_width - this->size.x;
        }
        if (this->position.y <= 0.0f)
        {
            this->velocity.y = -this->velocity.y;
            this->position.y = 0.0f;
        }
    }
    return this->position;
}

// Reseta a bola para a posi??o que ela t? presa
void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
    this->position = position;
    this->velocity = velocity;
    this->stuck = true;
    this->sticky = false;
    this->pass = false;
}