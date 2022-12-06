#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"
#include "GameObj.h"


// UMA particula
struct Particle {
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    float     Life;

    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

class ParticleGenerator
{
public:
    // Construtor
    ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
    // Update de todas as particulas
    void Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    // Renderiza todas as particulas
    void Draw();
private:
    std::vector<Particle> particles;
    unsigned int amount;
    // Render 
    Shader shader;
    Texture2D texture;
    unsigned int VAO;
    // Inicializa atributos de buffer e vertex 
    void init();
    // Retorna o index da primeira particula que não está sendo usada
    unsigned int firstUnusedParticle();
    // Respawn 
    void respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif