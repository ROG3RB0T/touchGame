#ifndef TOUCHGAME_GAME_H
#define TOUCHGAME_GAME_H

#include <GLES2/gl2.h>
#include <random>
#include <chrono>
#include <vector>
#include <functional>

// Callback function type for showing toasts
typedef std::function<void(const char*)> ToastCallback;

struct Particle {
    float x;
    float y;
    float velocityX;
    float velocityY;
    float size;
    float colorR;
    float colorG;
    float colorB;
    float lifetime; // seconds
    float maxLifetime;
};

struct Circle {
    float x;
    float y;
    float radius;
    float velocityX;
    float velocityY;
    float colorR;
    float colorG;
    float colorB;
    bool isDecoy; // true = explodes, false = scores points
    float flashTimer; // for visual feedback on touch
};

class Game {
public:
    Game();
    ~Game();
    
    void init(int screenWidth, int screenHeight);
    void update(float deltaTime);
    void render();
    bool handleTouch(float x, float y);
    void reset();
    
    void setToastCallback(ToastCallback callback) { toastCallback = callback; }
    
    int getScore() const { return score; }
    int getRound() const { return round; }
    bool isGameOver() const { return gameOver; }
    float getBgColorR1() const { return bgColorR1; }
    float getBgColorG1() const { return bgColorG1; }
    float getBgColorB1() const { return bgColorB1; }
    float getBgColorR2() const { return bgColorR2; }
    float getBgColorG2() const { return bgColorG2; }
    float getBgColorB2() const { return bgColorB2; }
    
private:
    void resetCircle();
    bool checkCollision(float touchX, float touchY, const Circle& circle);
    void setupShaders();
    void renderCircle(const Circle& circle);
    void renderParticle(const Particle& particle);
    void createExplosion(float x, float y, float radius, float r, float g, float b);
    
    // Multiple circles support
    std::vector<Circle> circles;
    std::vector<Particle> particles;
    
    // Circle properties
    float baseSpeed;
    float baseRadius;
    float bgColorR1;
    float bgColorG1;
    float bgColorB1;
    float bgColorR2;
    float bgColorG2;
    float bgColorB2;
    
    // Game state
    int score;
    int round;
    bool gameOver;
    
    // Screen dimensions
    int screenWidth;
    int screenHeight;
    
    // Random number generation
    std::mt19937 rng;
    std::uniform_real_distribution<float> distX;
    std::uniform_real_distribution<float> distY;
    std::uniform_real_distribution<float> distAngle;
    std::uniform_real_distribution<float> distColor;
    std::uniform_int_distribution<int> distDecoyCount;
    
    // OpenGL resources
    GLuint shaderProgram;
    GLuint vbo;
    GLint positionLoc;
    GLint colorLoc;
    GLint mvpLoc;
    GLuint gradientShaderProgram;
    GLuint gradientVbo;
    GLint gradientPositionLoc;
    GLint gradientColor1Loc;
    GLint gradientColor2Loc;
    
    // Callback for showing toast messages
    ToastCallback toastCallback;
};

#endif // TOUCHGAME_GAME_H
