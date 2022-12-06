#include "Game.h"
#include "SpriteRenderer.h"
#include "ResMan.h"
#include "BallObj.h"
#include "ParticleGen.h"
#include "PostProc.h"
#include "TextRenderer.h"

#include <irrKlang/irrKlang.h>
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace irrklang;


// Tamanho inicial do player
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// Velocidade inicial do player
const float PLAYER_VELOCITY(500.0f);

// Velocidade inicial da bola
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Raio da bola
const float BALL_RADIUS = 12.5f;

GameObject* Player;
BallObject* Ball;
SpriteRenderer* Renderer;
ParticleGenerator* Particle;
PostProcessor* Effects;
ISoundEngine* SoundEngine = createIrrKlangDevice();
TextRenderer* Text;


float ShakeTime = 0.0f;

Game::Game(unsigned int w, unsigned int h)
    : state(GAME_MENU), keys(), width(w), height(h), Lives(3)
{

}

Game::~Game()
{
    delete Renderer;
    delete Player;
    delete Ball;
    delete Particle;
}

void Game::Init()
{
    // Carrega os shaders
    ResourceManager::LoadShader("src/Shaders/sprite.vs", "src/Shaders/sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader("src/Shaders/particle.vs", "src/Shaders/particle.frag", nullptr, "particle");
    ResourceManager::LoadShader("src/Shaders/postprocess.vs", "src/Shaders/postprocess.frag", nullptr, "postprocessing");
    // Configura os shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->width),
        static_cast<float>(this->height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
    
    Shader myShader;
    myShader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(myShader);
    // Carrega textures
    ResourceManager::LoadTexture("src/Textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("src/Textures/ball.png", true, "face");
    ResourceManager::LoadTexture("src/Textures/block.png", false, "block");
    ResourceManager::LoadTexture("src/Textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("src/Textures/paddle.png", true, "paddle");
    ResourceManager::LoadTexture("src/Textures/particle.png", true, "particle");
    ResourceManager::LoadTexture("src/Textures/PwrUps/powerup_speed.png", true, "speed");
    ResourceManager::LoadTexture("src/Textures/PwrUps/powerup_sticky.png", true, "sticky");
    ResourceManager::LoadTexture("src/Textures/PwrUps/powerup_passthrough.png", true, "pass");
    ResourceManager::LoadTexture("src/Textures/PwrUps/powerup_increase.png", true, "size");
    ResourceManager::LoadTexture("src/Textures/PwrUps/powerup_confuse.png", true, "confuse");
    ResourceManager::LoadTexture("src/Textures/PwrUps/powerup_chaos.png", true, "chaos");
    // Carrega levels
    GameLevel one; 
    one.Load("src/Levels/one.lvl", this->width, this->height / 2);
    GameLevel two; 
    two.Load("src/Levels/two.lvl", this->width, this->height / 2);
    GameLevel three; 
    three.Load("src/Levels/three.lvl", this->width, this->height / 2);
    GameLevel four; 
    four.Load("src/Levels/four.lvl", this->width, this->height / 2);
    GameLevel five;
    five.Load("src/Levels/five.lvl", this->width, this->height / 2);
    GameLevel six;
    six.Load("src/Levels/six.lvl", this->width, this->height / 2);
    GameLevel seven;
    seven.Load("src/Levels/seven.lvl", this->width, this->height / 2);
    GameLevel eight;
    eight.Load("src/Levels/eight.lvl", this->width, this->height / 2);
    GameLevel nine;
    nine.Load("src/Levels/nine.lvl", this->width, this->height / 2);
    GameLevel ten;
    ten.Load("src/Levels/ten.lvl", this->width, this->height / 2);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Levels.push_back(five);
    this->Levels.push_back(six);
    this->Levels.push_back(seven);
    this->Levels.push_back(eight);
    this->Levels.push_back(nine);
    this->Levels.push_back(ten);
    this->Level = 0;
    //Seta a pos do player
    glm::vec2 playerPos = glm::vec2(
        this->width / 2.0f - PLAYER_SIZE.x / 2.0f,
        this->height - PLAYER_SIZE.y
    );
    Texture2D ptxt;
    ptxt = ResourceManager::GetTexture("paddle");
    Player = new GameObject(playerPos, PLAYER_SIZE, ptxt);
    //Seta a bola
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
        -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
        ResourceManager::GetTexture("face"));
    //Seta as particulas
    Particle = new ParticleGenerator(
        ResourceManager::GetShader("particle"),
        ResourceManager::GetTexture("particle"),
        5000
    );
    //Seta o post process
    Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->width, this->height);
    SoundEngine->play2D("src/Audio/breakout.mp3", true);
    //Seta a caceta do texto
    Text = new TextRenderer(this->width, this->height);
    Text->Load("src/Fonts/big-shot.ttf", 24);

}

void Game::Update(float dt)
{
    //Controla o movimento da bola
    Ball->Move(dt, this->width);
    //Faz as colisões
    this->DoCollisions();
    if (Ball->position.y >= this->height) // Bola chegou embaixo?
    {
        --this->Lives;
        if (this->Lives == 0)
        {
            this->ResetLevel();
            this->state = GAME_MENU;
        }
        this->ResetPlayer();
    }
    //Update dos powerups
    this->UpdatePowerUps(dt);
    //Update das particulas
    Particle->Update(dt, *Ball, 2, glm::vec2(Ball->radius / 2.0f));
    //Controle do shake
    if (ShakeTime > 0.0f)
    {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f) {
            Effects->Shake = false;
        }
    }

    //Ganhando essa bomba
    if (this->state == GAME_ACTIVE && this->Levels[this->Level].IsCompleted())
    {
        this->ResetLevel();
        this->ResetPlayer();
        Effects->Chaos = true;
        this->state = GAME_WIN;
    }
}

void Game::ProcessInput(float dt)
{
    //Teclas do jogo
    if (this->state == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if (this->keys[GLFW_KEY_A])
        {
            if (Player->position.x >= 0.0f) {
                Player->position.x -= velocity;
                if (Ball->stuck) {
                    Ball->position.x -= velocity;
                }
            }
        }
        if (this->keys[GLFW_KEY_D])
        {
            if (Player->position.x <= this->width - Player->size.x) {
                Player->position.x += velocity;
                if (Ball->stuck) {
                    Ball->position.x += velocity;
                }
            }
        }
        if (this->keys[GLFW_KEY_SPACE]) 
        {
            Ball->stuck = false;
        }
    }
    //Teclas do menu
    if (this->state == GAME_MENU)
    {
        if (this->keys[GLFW_KEY_ENTER] && !this->keysProcessed[GLFW_KEY_ENTER])
        {
            this->state = GAME_ACTIVE;
            this->keysProcessed[GLFW_KEY_ENTER] = true;
        }
        if (this->keys[GLFW_KEY_W] && !this->keysProcessed[GLFW_KEY_W])
        {
            this->Level = (this->Level + 1) % 10;
            this->keysProcessed[GLFW_KEY_W] = true;
        }
        if (this->keys[GLFW_KEY_S] && !this->keysProcessed[GLFW_KEY_S])
        {
            if (this->Level > 0) 
            {
                --this->Level;
            }
            else 
            {
                this->Level = 3;
            }
            this->keysProcessed[GLFW_KEY_S] = true;
        }

    }
    //Teclas da vitória
    if (this->state == GAME_WIN)
    {
        if (this->keys[GLFW_KEY_ENTER])
        {
            this->keysProcessed[GLFW_KEY_ENTER] = true;
            Effects->Chaos = false;
            this->state = GAME_MENU;
        }
    }

}

void Game::Render()
{
    if (this->state == GAME_ACTIVE || this->state == GAME_MENU || this->state == GAME_WIN)
    {
        Effects->BeginRender();
        // Desenha fundo
        Texture2D bg;
        bg = ResourceManager::GetTexture("background");
        Renderer->DrawSprite(bg, glm::vec2(0.0f, 0.0f), glm::vec2(this->width, this->height), 0.0f);
        // Desenha o level
        this->Levels[this->Level].Draw(*Renderer);
        // Desenha o player
        Player->Draw(*Renderer);
        //Desenha as particulas
        if (!Ball->stuck) {
            Particle->Draw();
        }
        //Desenha a bola
        Ball->Draw(*Renderer);
        //Desenha os power ups
        for (PowerUp& powerUp : this->PowerUps)
        {
            if (!powerUp.destroyed) 
            {
                powerUp.Draw(*Renderer);
            }
        }
        //Desenha o post process
        Effects->EndRender();
        Effects->Render(glfwGetTime());
        //Render texto de vida
        std::stringstream ss; 
        ss << this->Lives;
        Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
        //Render texto de vencedor
        if (this->state == GAME_WIN)
        {
            Text->RenderText(
                "GANHASTES!!! POGGERS!!!", (width/2) - 200.0f, height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0)
            );
            Text->RenderText(
                "Aperte ENTER para restartar os ESC para sair", (width / 2) - 350.0f, (height / 2) + 10, 1.0, glm::vec3(1.0, 1.0, 0.0)
            );
        }
    }
    if (this->state == GAME_MENU)
    {
        Text->RenderText("Aperte ENTER para jogar!", (width / 2) - 200.0f, height / 2, 1.0f);
        Text->RenderText("Aperte W ou S para selecionar o level", (width / 2) - 225.0f, (height / 2) + 30.0f, 0.75f);
    }
}

bool ShouldSpawn(unsigned int chance)
{
    unsigned int random = rand() % chance;
    return random == 0;
}

void Game::SpawnPowerUps(GameObject& block)
{
    if (ShouldSpawn(75)) // 1 em 75 
        this->PowerUps.push_back(
            PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.position, ResourceManager::GetTexture("speed")
            ));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(
            PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.position, ResourceManager::GetTexture("sticky")
            ));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(
            PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.position, ResourceManager::GetTexture("pass")
            ));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(
            PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.position, ResourceManager::GetTexture("size")
            ));
    if (ShouldSpawn(15)) 
        this->PowerUps.push_back(
            PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 5.0f, block.position, ResourceManager::GetTexture("confuse")
            ));
    if (ShouldSpawn(30))
        this->PowerUps.push_back(
            PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 5.0f, block.position, ResourceManager::GetTexture("chaos")
            ));
}


void Game::ResetLevel()
{
    switch (this->Level)
    {
    case 0:
        this->Levels[0].Load("src/Levels/one.lvl", this->width, this->height / 2);
        break;
    case 1:
        this->Levels[1].Load("src/Levels/two.lvl", this->width, this->height / 2);
        break;
    case 2:
        this->Levels[2].Load("src/Levels/three.lvl", this->width, this->height / 2);
        break;
    case 3:
        this->Levels[3].Load("src/Levels/four.lvl", this->width, this->height / 2);
        break;
    case 4:
        this->Levels[4].Load("src/Levels/five.lvl", this->width, this->height / 2);
        break;
    case 5:
        this->Levels[5].Load("src/Levels/six.lvl", this->width, this->height / 2);
        break;
    case 6:
        this->Levels[6].Load("src/Levels/seven.lvl", this->width, this->height / 2);
        break;
    case 7:
        this->Levels[7].Load("src/Levels/eight.lvl", this->width, this->height / 2);
        break;
    case 8:
        this->Levels[8].Load("src/Levels/nine.lvl", this->width, this->height / 2);
        break;
    case 9:
        this->Levels[9].Load("src/Levels/ten.lvl", this->width, this->height / 2);
        break;
    default:
        break;
    }
    this->Lives = 3;
}

void ActivatePowerUp(PowerUp& powerUp)
{
    if (powerUp.type == "speed")
    {
        Ball->velocity *= 1.2;
    }
    else if (powerUp.type == "sticky")
    {
        Ball->sticky = true;
        Player->color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (powerUp.type == "pass-through")
    {
        Ball->pass = true;
        Ball->color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (powerUp.type == "pad-size-increase")
    {
        Player->size.x += 50;
    }
    else if (powerUp.type == "confuse")
    {
        if (!Effects->Chaos)
            Effects->Confuse = true; // only activate if chaos wasn't already active
    }
    else if (powerUp.type == "chaos")
    {
        if (!Effects->Confuse)
            Effects->Chaos = true;
    }
}


void Game::ResetPlayer()
{
    // Reseta o status do player/bola
    Player->size = PLAYER_SIZE;
    Player->position = glm::vec2(this->width / 2.0f - PLAYER_SIZE.x / 2.0f, this->height - PLAYER_SIZE.y);
    Ball->Reset(Player->position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
    Effects->Chaos = Effects->Confuse = false;
    Ball->pass = Ball->sticky = false;
}


bool CheckCollision(GameObject& one, GameObject& two);
Collision CheckCollision(BallObject& one, GameObject& two);
Direction VectorDirection(glm::vec2 closest);


void Game::DoCollisions()
{
    for (GameObject& box : this->Levels[this->Level].Bricks)
    {
        if (!box.destroyed)
        {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) // if collision is true
            {
                // destroy block if not solid
                if (!box.isSolid)
                {
                    box.destroyed = true;
                    this->SpawnPowerUps(box);
                    SoundEngine->play2D("src/Audio/bleepblock.mp3", false);
                }
                else {
                    ShakeTime = 0.05f;
                    Effects->Shake = true;
                    SoundEngine->play2D("src/Audio/solid.wav", false);
                }
                // collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (!(Ball->pass && !box.isSolid))
                {
                    if (dir == LEFT || dir == RIGHT) // Colisão horizontal
                    {
                        Ball->velocity.x = -Ball->velocity.x; // Reverte a vel hor
                        // relocate
                        float penetration = Ball->radius - std::abs(diff_vector.x);
                        if (dir == LEFT)
                            Ball->position.x += penetration;
                        else
                            Ball->position.x -= penetration;
                    }
                    else // Colisão vertical
                    {
                        Ball->velocity.y = -Ball->velocity.y; // Reverte a vel vert
                        // relocate
                        float penetration = Ball->radius - std::abs(diff_vector.y);
                        if (dir == UP)
                            Ball->position.y -= penetration; // Bola pra cima
                        else
                            Ball->position.y += penetration; // Bola pra baixo
                    }
                }
            }
        }
        for (PowerUp& powerUp : this->PowerUps)
        {
            if (!powerUp.destroyed)
            {
                if (powerUp.position.y >= this->height)
                    powerUp.destroyed = true;
                if (CheckCollision(*Player, powerUp))
                {
                    ActivatePowerUp(powerUp);
                    powerUp.destroyed = true;
                    powerUp.activated = true;
                    SoundEngine->play2D("src/Audio/powerup.wav", false);
                }
            }
        }

        // Checa colisões para a pad
        Collision result = CheckCollision(*Ball, *Player);
        if (!Ball->stuck && std::get<0>(result))
        {
            // Checa onde atingiu a pad e muda a velociade baseada onde antige 
            float centerBoard = Player->position.x + Player->size.x / 2.0f;
            float distance = (Ball->position.x + Ball->radius) - centerBoard;
            float percentage = distance / (Player->size.x / 2.0f);
            float strength = 2.0f;
            glm::vec2 oldVelocity = Ball->velocity;
            Ball->velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
            Ball->velocity = glm::normalize(Ball->velocity) * glm::length(oldVelocity); // Mantem a vel consistente
            Ball->velocity.y = -1.0f * abs(Ball->velocity.y);
            
            // Powerup sticky
            Ball->stuck = Ball->sticky;
            SoundEngine->play2D("src/Audio/bleeppad.wav", false);
        }

    }
}

bool IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type)
{
    for (const PowerUp& powerUp : powerUps)
    {
        if (powerUp.activated)
            if (powerUp.type == type)
                return true;
    }
    return false;
}


void Game::UpdatePowerUps(float dt)
{
    for (PowerUp& powerUp : this->PowerUps)
    {
        powerUp.position += powerUp.velocity * dt;
        if (powerUp.activated)
        {
            powerUp.duration -= dt;

            if (powerUp.duration <= 0.0f)
            {
                // Remove powerup da lista 
                powerUp.activated = false;
                // Desativa
                if (powerUp.type == "sticky")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "sticky"))
                    {
                        Ball->sticky = false;
                        Player->color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.type == "pass-through")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "pass-through"))
                    {
                        Ball->pass = false;
                        Ball->color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.type == "confuse")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
                    {
                        Effects->Confuse = false;
                    }
                }
                else if (powerUp.type == "chaos")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
                    {	
                        Effects->Chaos = false;
                    }
                }
            }
        }
    }
    this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
        [](const PowerUp& powerUp) { return powerUp.destroyed && !powerUp.activated; }
    ), this->PowerUps.end());
}


//Colisão quadrada
bool CheckCollision(GameObject& one, GameObject& two)
{
    // Collision x-axis?
    bool collisionX = one.position.x + one.size.x >= two.position.x &&
        two.position.x + two.size.x >= one.position.x;
    // Collision y-axis?
    bool collisionY = one.position.y + one.size.y >= two.position.y &&
        two.position.y + two.size.y >= one.position.y;
    // Collision retorna somente se os dois eixos colidirem
    return collisionX && collisionY;
}

//Colisão circular
Collision CheckCollision(BallObject& one, GameObject& two)
{
    // Pega o centro do ciruclo
    glm::vec2 center(one.position + one.radius);
    // Calcula a info do AABB
    glm::vec2 aabb_half_extents(two.size.x / 2.0f, two.size.y / 2.0f);
    glm::vec2 aabb_center(
        two.position.x + aabb_half_extents.x,
        two.position.y + aabb_half_extents.y
    );
    // Pega a diferença entre os dois centros
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // Soma o valor clampado e pega o quad mais próximo do circulo
    glm::vec2 closest = aabb_center + clamped;
    // Pega o vetor do quad mais próximo do circulo e checa se a distância <= raio
    difference = closest - center;

    if (glm::length(difference) < one.radius)
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

//Calcula a direção do vetor de direção (N,S,L,O)
Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}