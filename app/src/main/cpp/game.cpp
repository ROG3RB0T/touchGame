#include "game.h"
#include <cmath>
#include <android/log.h>

#define LOG_TAG "TouchGame"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

const char* vertexShaderSource = R"(
    attribute vec2 position;
    uniform mat4 mvp;
    void main() {
        gl_Position = mvp * vec4(position, 0.0, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    precision mediump float;
    uniform vec4 color;
    void main() {
        gl_FragColor = color;
    }
)";

const char* gradientVertexShaderSource = R"(
    attribute vec2 position;
    varying vec2 vPosition;
    void main() {
        gl_Position = vec4(position, 0.0, 1.0);
        vPosition = position;
    }
)";

const char* gradientFragmentShaderSource = R"(
    precision mediump float;
    varying vec2 vPosition;
    uniform vec4 color1;
    uniform vec4 color2;
    void main() {
        float gradient = (vPosition.y + 1.0) * 0.5;
        gl_FragColor = mix(color1, color2, gradient);
    }
)";

Game::Game() : score(0), round(1), gameOver(false), baseSpeed(500.0f), 
               baseRadius(0.0f), shaderProgram(0), vbo(0),
               gradientShaderProgram(0), gradientVbo(0) {
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    rng.seed(seed);
    distAngle = std::uniform_real_distribution<float>(0, 2 * M_PI);
    distColor = std::uniform_real_distribution<float>(0.0f, 1.0f);
    distDecoyCount = std::uniform_int_distribution<int>(1, 10);
    // Initialize with light gradient
    bgColorR1 = 0.9f;
    bgColorG1 = 0.9f;
    bgColorB1 = 1.0f;
    bgColorR2 = 1.0f;
    bgColorG2 = 0.9f;
    bgColorB2 = 0.9f;
}

Game::~Game() {
    if (shaderProgram) {
        glDeleteProgram(shaderProgram);
    }
    if (vbo) {
        glDeleteBuffers(1, &vbo);
    }
    if (gradientShaderProgram) {
        glDeleteProgram(gradientShaderProgram);
    }
    if (gradientVbo) {
        glDeleteBuffers(1, &gradientVbo);
    }
}

void Game::init(int width, int height) {
    screenWidth = width;
    screenHeight = height;
    
    // Set base radius proportional to screen size (10% of smaller dimension)
    float minDimension = (width < height) ? width : height;
    baseRadius = minDimension * 0.10f;
    
    distX = std::uniform_real_distribution<float>(baseRadius, width - baseRadius);
    distY = std::uniform_real_distribution<float>(baseRadius, height - baseRadius);
    distAngle = std::uniform_real_distribution<float>(0.0f, 2.0f * M_PI);
    
    setupShaders();
    resetCircle();
    
    LOGI("Game initialized: %dx%d, baseRadius: %.1f", width, height, baseRadius);
}

void Game::setupShaders() {
    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    
    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    
    // Link program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Get attribute and uniform locations
    positionLoc = glGetAttribLocation(shaderProgram, "position");
    colorLoc = glGetUniformLocation(shaderProgram, "color");
    mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
    
    // Create VBO
    glGenBuffers(1, &vbo);
    
    // Setup gradient shader
    GLuint gradientVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(gradientVertexShader, 1, &gradientVertexShaderSource, nullptr);
    glCompileShader(gradientVertexShader);
    
    GLuint gradientFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(gradientFragmentShader, 1, &gradientFragmentShaderSource, nullptr);
    glCompileShader(gradientFragmentShader);
    
    gradientShaderProgram = glCreateProgram();
    glAttachShader(gradientShaderProgram, gradientVertexShader);
    glAttachShader(gradientShaderProgram, gradientFragmentShader);
    glLinkProgram(gradientShaderProgram);
    
    glDeleteShader(gradientVertexShader);
    glDeleteShader(gradientFragmentShader);
    
    gradientPositionLoc = glGetAttribLocation(gradientShaderProgram, "position");
    gradientColor1Loc = glGetUniformLocation(gradientShaderProgram, "color1");
    gradientColor2Loc = glGetUniformLocation(gradientShaderProgram, "color2");
    
    glGenBuffers(1, &gradientVbo);
}

void Game::resetCircle() {
    circles.clear();
    
    // Shrink circle by 5% each round (minimum 3% of screen size)
    float minDimension = (screenWidth < screenHeight) ? screenWidth : screenHeight;
    float minRadius = minDimension * 0.03f;
    
    float circleRadius = baseRadius * powf(0.95f, round - 1);
    if (circleRadius < minRadius) circleRadius = minRadius;
    
    // Update distributions with new radius
    distX = std::uniform_real_distribution<float>(circleRadius, screenWidth - circleRadius);
    distY = std::uniform_real_distribution<float>(circleRadius, screenHeight - circleRadius);
    
    // Generate random gradient background colors for each level
    bgColorR1 = distColor(rng) * 0.7f + 0.3f; // Keep colors bright (0.3-1.0)
    bgColorG1 = distColor(rng) * 0.7f + 0.3f;
    bgColorB1 = distColor(rng) * 0.7f + 0.3f;
    bgColorR2 = distColor(rng) * 0.7f + 0.3f;
    bgColorG2 = distColor(rng) * 0.7f + 0.3f;
    bgColorB2 = distColor(rng) * 0.7f + 0.3f;
    
    // Calculate average background brightness
    float avgBrightness = (bgColorR1 + bgColorG1 + bgColorB1 + bgColorR2 + bgColorG2 + bgColorB2) / 6.0f;
    
    float speed = baseSpeed * (1.0f + (round - 1) * 0.2f);
    
    // Number of circles increases with each level
    int totalCircles;
    if (round < 11) {
        totalCircles = round;
    } else {
        // From level 11 onwards, random 2-10 circles
        totalCircles = 2 + (rand() % 9); // 2-10 circles
    }
    
    // Create all circles
    for (int i = 0; i < totalCircles; i++) {
        Circle circle;
        circle.x = distX(rng);
        circle.y = distY(rng);
        circle.radius = circleRadius;
        circle.flashTimer = 0.0f;
        circle.isDecoy = false; // All circles are valid targets
        
        // Generate contrasting circle color (dark if bg is bright, bright if bg is dark)
        if (avgBrightness > 0.5f) {
            // Dark circle for bright background
            circle.colorR = distColor(rng) * 0.4f;
            circle.colorG = distColor(rng) * 0.4f;
            circle.colorB = distColor(rng) * 0.4f;
        } else {
            // Bright circle for dark background
            circle.colorR = distColor(rng) * 0.4f + 0.6f;
            circle.colorG = distColor(rng) * 0.4f + 0.6f;
            circle.colorB = distColor(rng) * 0.4f + 0.6f;
        }
        
        float angle = distAngle(rng);
        circle.velocityX = cos(angle) * speed;
        circle.velocityY = sin(angle) * speed;
        
        circles.push_back(circle);
    }
    
    LOGI("Circle reset: %d circles, radius=%.1f, speed=%.1f, round=%d", 
         totalCircles, circleRadius, speed, round);
}

void Game::update(float deltaTime) {
    if (gameOver) return;
    
    // Update all circles
    for (auto& circle : circles) {
        // Update position
        circle.x += circle.velocityX * deltaTime;
        circle.y += circle.velocityY * deltaTime;
        
        // Update flash timer for visual feedback
        if (circle.flashTimer > 0.0f) {
            circle.flashTimer -= deltaTime;
        }
        
        // Bounce off walls
        if (circle.x - circle.radius < 0 || circle.x + circle.radius > screenWidth) {
            circle.velocityX = -circle.velocityX;
            circle.x = std::max(circle.radius, std::min(circle.x, screenWidth - circle.radius));
        }
        
        if (circle.y - circle.radius < 0 || circle.y + circle.radius > screenHeight) {
            circle.velocityY = -circle.velocityY;
            circle.y = std::max(circle.radius, std::min(circle.y, screenHeight - circle.radius));
        }
    }
    
    // Update particles
    for (auto it = particles.begin(); it != particles.end();) {
        it->x += it->velocityX * deltaTime;
        it->y += it->velocityY * deltaTime;
        it->velocityY += 600.0f * deltaTime; // Gravity
        it->lifetime += deltaTime;
        
        // Remove dead particles
        if (it->lifetime >= it->maxLifetime) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Render gradient background
    glUseProgram(gradientShaderProgram);
    
    // Full screen quad vertices in normalized device coordinates
    float quadVertices[] = {
        -1.0f, -1.0f,  // Bottom left
         1.0f, -1.0f,  // Bottom right
        -1.0f,  1.0f,  // Top left
         1.0f,  1.0f   // Top right
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, gradientVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(gradientPositionLoc);
    glVertexAttribPointer(gradientPositionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    // Set gradient colors
    glUniform4f(gradientColor1Loc, bgColorR1, bgColorG1, bgColorB1, 1.0f);
    glUniform4f(gradientColor2Loc, bgColorR2, bgColorG2, bgColorB2, 1.0f);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(gradientPositionLoc);
    
    // Render all circles
    for (const auto& circle : circles) {
        renderCircle(circle);
    }
    
    // Render all particles
    for (const auto& particle : particles) {
        renderParticle(particle);
    }
}

void Game::renderCircle(const Circle& circle) {
    glUseProgram(shaderProgram);
    
    // Create orthographic projection matrix
    float left = 0.0f;
    float right = static_cast<float>(screenWidth);
    float bottom = static_cast<float>(screenHeight);
    float top = 0.0f;
    float near = -1.0f;
    float far = 1.0f;
    
    float ortho[16] = {
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, -2.0f / (far - near), 0.0f,
        -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1.0f
    };
    
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, ortho);
    
    // Draw 3D ball with realistic lighting and shading
    const int segments = 48;
    const int rings = 24; // More rings for ultra-smooth gradient
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(positionLoc);
    glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    // Light position (center of screen)
    float screenCenterX = screenWidth / 2.0f;
    float screenCenterY = screenHeight / 2.0f;
    
    // Calculate direction from circle to light
    float toLightX = screenCenterX - circle.x;
    float toLightY = screenCenterY - circle.y;
    float toLightDist = sqrtf(toLightX * toLightX + toLightY * toLightY);
    
    // Normalize and scale to create highlight offset
    float lightOffsetX, lightOffsetY;
    if (toLightDist > 0.01f) {
        float normalizedX = toLightX / toLightDist;
        float normalizedY = toLightY / toLightDist;
        lightOffsetX = normalizedX * circle.radius * 0.3f;
        lightOffsetY = normalizedY * circle.radius * 0.3f;
    } else {
        // Circle is at screen center, default to top offset
        lightOffsetX = 0.0f;
        lightOffsetY = -circle.radius * 0.3f;
    }
    
    // Draw from center to outer edge with spherical gradient
    for (int ring = 0; ring < rings; ring++) {
        float innerRadius = (ring / (float)rings) * circle.radius;
        float outerRadius = ((ring + 1) / (float)rings) * circle.radius;
        
        float vertices[(segments + 1) * 4];
        int vertexIndex = 0;
        
        for (int i = 0; i <= segments; i++) {
            float angle = 2.0f * M_PI * i / segments;
            float cosAngle = cos(angle);
            float sinAngle = sin(angle);
            
            // Inner vertex
            vertices[vertexIndex++] = circle.x + innerRadius * cosAngle;
            vertices[vertexIndex++] = circle.y + innerRadius * sinAngle;
            
            // Outer vertex
            vertices[vertexIndex++] = circle.x + outerRadius * cosAngle;
            vertices[vertexIndex++] = circle.y + outerRadius * sinAngle;
        }
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        
        // Calculate distance from light position for each ring
        float ringCenterRadius = (innerRadius + outerRadius) / 2.0f;
        
        // Simulate spherical lighting using distance from light
        // For each point on the ring, calculate brightness based on sphere geometry
        float normalizedRadius = ringCenterRadius / circle.radius;
        
        // Spherical brightness calculation with smoother transitions
        // z = sqrt(1 - x² - y²) for a sphere, simplified for 2D circle
        float z = sqrtf(fmax(0.0f, 1.0f - normalizedRadius * normalizedRadius));
        
        // Distance from light position (offset highlight)
        float lightDist = sqrtf(
            (lightOffsetX * lightOffsetX + lightOffsetY * lightOffsetY) / 
            (circle.radius * circle.radius)
        );
        
        // Enhanced brightness calculation with smoother falloff
        float brightness;
        if (ring == 0) {
            // Brightest specular highlight at center
            brightness = 2.4f;
        } else if (normalizedRadius < 0.15f) {
            // Very bright core area with smooth transition
            float t = normalizedRadius / 0.15f;
            brightness = 2.4f - t * 0.6f; // 2.4 to 1.8
        } else if (normalizedRadius < 0.35f) {
            // Bright highlight area
            float t = (normalizedRadius - 0.15f) / 0.2f;
            brightness = 1.8f - t * 0.4f; // 1.8 to 1.4
        } else if (normalizedRadius < 0.6f) {
            // Mid-tones with gradual falloff
            float t = (normalizedRadius - 0.35f) / 0.25f;
            t = t * t; // Quadratic for smoother transition
            brightness = 1.4f - t * 0.35f; // 1.4 to 1.05
        } else if (normalizedRadius < 0.85f) {
            // Shadow area
            float t = (normalizedRadius - 0.6f) / 0.25f;
            t = t * t; // Quadratic for smoother transition
            brightness = 1.05f - t * 0.35f; // 1.05 to 0.7
        } else {
            // Dark edge with smooth ambient occlusion
            float t = (normalizedRadius - 0.85f) / 0.15f;
            t = t * t * t; // Cubic for very smooth edge
            brightness = 0.7f - t * 0.35f; // 0.7 to 0.35
            
            // Enhanced rim lighting effect
            if (normalizedRadius > 0.9f) {
                float rimStrength = (normalizedRadius - 0.9f) / 0.1f;
                rimStrength = rimStrength * rimStrength; // Quadratic for smooth rim
                brightness += rimStrength * 0.25f; // Stronger rim light
            }
        }
        
        // Add ambient light so it's never completely black
        brightness = brightness * 0.88f + 0.12f;
        
        // Add flash effect when circle is touched
        if (circle.flashTimer > 0.0f) {
            float flashStrength = circle.flashTimer / 0.15f; // 0.15 second flash duration
            brightness += flashStrength * 0.8f; // Bright flash
        }
        
        // Apply brightness to color
        float r = circle.colorR * brightness;
        float g = circle.colorG * brightness;
        float b = circle.colorB * brightness;
        
        // Clamp values
        r = r > 1.0f ? 1.0f : r;
        g = g > 1.0f ? 1.0f : g;
        b = b > 1.0f ? 1.0f : b;
        
        glUniform4f(colorLoc, r, g, b, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (segments + 1) * 2);
    }
    
    // Add glossy highlight spot with smoother edges
    const int highlightSegments = 28;
    float highlightRadius = circle.radius * 0.22f;
    float highlightX = circle.x + lightOffsetX;
    float highlightY = circle.y + lightOffsetY;
    
    float highlightVertices[highlightSegments * 2 + 4];
    highlightVertices[0] = highlightX;
    highlightVertices[1] = highlightY;
    
    for (int i = 0; i <= highlightSegments; i++) {
        float angle = 2.0f * M_PI * i / highlightSegments;
        highlightVertices[2 + i * 2] = highlightX + highlightRadius * cos(angle);
        highlightVertices[3 + i * 2] = highlightY + highlightRadius * sin(angle);
    }
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(highlightVertices), highlightVertices, GL_DYNAMIC_DRAW);
    
    // White glossy highlight with some transparency
    glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 0.6f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLE_FAN, 0, highlightSegments + 2);
    glDisable(GL_BLEND);
    
    glDisableVertexAttribArray(positionLoc);
}

void Game::renderParticle(const Particle& particle) {
    glUseProgram(shaderProgram);
    
    float ortho[16] = {
        2.0f / screenWidth, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / screenHeight, 0.0f, 0.0f,
        0.0f, 0.0f, -2.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f
    };
    
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, ortho);
    
    // Draw particle as a small square
    float halfSize = particle.size / 2.0f;
    float vertices[] = {
        particle.x - halfSize, particle.y - halfSize,
        particle.x + halfSize, particle.y - halfSize,
        particle.x + halfSize, particle.y + halfSize,
        particle.x - halfSize, particle.y + halfSize
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(positionLoc);
    glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    // Fade out based on lifetime
    float alpha = 1.0f - (particle.lifetime / particle.maxLifetime);
    glUniform4f(colorLoc, particle.colorR, particle.colorG, particle.colorB, alpha);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glDisableVertexAttribArray(positionLoc);
}

void Game::createExplosion(float x, float y, float radius, float r, float g, float b) {
    // Create 20-30 particles flying outward
    int numParticles = 20 + (rand() % 11);
    
    for (int i = 0; i < numParticles; i++) {
        Particle p;
        p.x = x;
        p.y = y;
        
        // Random direction
        float angle = distAngle(rng);
        float speed = 200.0f + distColor(rng) * 400.0f; // 200-600 px/s
        
        p.velocityX = cos(angle) * speed;
        p.velocityY = sin(angle) * speed - 200.0f; // Initial upward bias
        
        p.size = radius * 0.15f + distColor(rng) * radius * 0.1f; // Variable sizes
        
        // Slight color variation
        p.colorR = std::min(1.0f, r + (distColor(rng) - 0.5f) * 0.2f);
        p.colorG = std::min(1.0f, g + (distColor(rng) - 0.5f) * 0.2f);
        p.colorB = std::min(1.0f, b + (distColor(rng) - 0.5f) * 0.2f);
        
        p.lifetime = 0.0f;
        p.maxLifetime = 0.5f + distColor(rng) * 0.5f; // 0.5-1.0 seconds
        
        particles.push_back(p);
    }
}

bool Game::handleTouch(float touchX, float touchY) {
    if (gameOver) return false;
    
    // Check all circles for collision
    for (size_t i = 0; i < circles.size(); i++) {
        if (checkCollision(touchX, touchY, circles[i])) {
            // Trigger visual flash feedback
            circles[i].flashTimer = 0.15f; // 150ms flash
            
            // Create explosion at touch position
            createExplosion(touchX, touchY, circles[i].radius,
                          circles[i].colorR, circles[i].colorG, circles[i].colorB);
            
            // Add points for this circle
            int points = 10;
            score += points;
            
            LOGI("Hit circle! Score: %d (+%d), Remaining: %zu", score, points, circles.size() - 1);
            
            // Remove the touched circle
            circles.erase(circles.begin() + i);
            
            // Check if all circles are cleared
            if (circles.empty()) {
                // Level complete - advance to next round
                round++;
                LOGI("Level complete! Advancing to round %d", round);
                
                // Show toast message
                if (toastCallback) {
                    char message[100];
                    snprintf(message, sizeof(message), "Level complete! Advancing to round %d", round);
                    toastCallback(message);
                }
                
                resetCircle();
            }
            
            return true;
        }
    }
    
    return false;
}

bool Game::checkCollision(float touchX, float touchY, const Circle& circle) {
    float dx = touchX - circle.x;
    float dy = touchY - circle.y;
    float distance = sqrt(dx * dx + dy * dy);
    
    // Increase touch tolerance by 50% for better detection
    float touchRadius = circle.radius * 1.5f;
    
    return distance <= touchRadius;
}

void Game::reset() {
    score = 0;
    round = 1;
    gameOver = false;
    resetCircle();
    LOGI("Game reset");
}
