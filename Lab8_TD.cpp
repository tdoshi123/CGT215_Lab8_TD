
// DuckHunter.cpp : Updated version of BalloonBuster to create a Duck Hunter game without sound.

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

// Constants for game configuration
const float KB_SPEED = 0.2;
const int MAX_SHOTS = 5;
const int DUCK_CREATION_INTERVAL_MS = 1000;  // Time interval to spawn ducks in milliseconds
const float DUCK_SPEED = 0.1;  // Duck movement speed
const Vector2f DUCK_START_POS(-50, 50);  // Starting position of ducks off-screen

// Load texture helper function
void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

// Main game function
int main() {
    RenderWindow window(VideoMode(800, 600), "Duck Hunter");
    window.setFramerateLimit(60);
    
    // Crossbow setup
    Texture crossbowTex;
    LoadTex(crossbowTex, "crossbow.png");  // Add appropriate path
    PhysicsSprite crossbow;
    crossbow.setTexture(crossbowTex);
    crossbow.setCenter(Vector2f(400, 500));  // Position at bottom of screen

    // Arrow setup
    Texture arrowTex;
    LoadTex(arrowTex, "arrow.png");  // Add appropriate path
    PhysicsShapeList<PhysicsSprite> arrows;

    // Duck setup
    Texture duckTex;
    LoadTex(duckTex, "duck.png");  // Use transparent background duck image
    PhysicsShapeList<PhysicsSprite> ducks;

    // Game state variables
    int shotsRemaining = MAX_SHOTS;
    int score = 0;
    bool gameOver = false;
    Clock duckSpawnClock;
    
    // Font and text for score and shots
    Font font;
    font.loadFromFile("arial.ttf");
    Text scoreText, shotsText, gameOverText;
    scoreText.setFont(font);
    shotsText.setFont(font);
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER - Press Space to Restart");
    gameOverText.setPosition(250, 300);
    
    // Game loop
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        // Restart game if game over and space is pressed
        if (gameOver && Keyboard::isKeyPressed(Keyboard::Space)) {
            gameOver = false;
            score = 0;
            shotsRemaining = MAX_SHOTS;
            ducks.clear();
            arrows.clear();
        }

        // Arrow shooting
        if (!gameOver && Keyboard::isKeyPressed(Keyboard::Space) && shotsRemaining > 0) {
            PhysicsSprite arrow;
            arrow.setTexture(arrowTex);
            arrow.setCenter(crossbow.getCenter());
            arrow.setVelocity(Vector2f(0, -0.3));  // Move arrow upward
            arrows.push_back(arrow);
            shotsRemaining--;
        }

        // Duck spawning
        if (duckSpawnClock.getElapsedTime().asMilliseconds() >= DUCK_CREATION_INTERVAL_MS) {
            PhysicsSprite duck;
            duck.setTexture(duckTex);
            duck.setCenter(DUCK_START_POS);
            duck.setVelocity(Vector2f(DUCK_SPEED, 0));  // Move duck to the right
            ducks.push_back(duck);
            duckSpawnClock.restart();
        }

        // Update arrow positions and detect collisions with ducks
        for (auto& arrow : arrows) {
            arrow.update();
            for (auto& duck : ducks) {
                if (arrow.getGlobalBounds().intersects(duck.getGlobalBounds())) {
                    ducks.remove(duck);  // Remove duck
                    arrows.remove(arrow);  // Remove arrow
                    score++;
                    break;  // Only one collision per arrow
                }
            }
        }

        // Remove ducks that reach the right side boundary
        for (auto& duck : ducks) {
            duck.update();
            if (duck.getCenter().x > window.getSize().x) {
                ducks.remove(duck);
            }
        }

        // End game condition
        if (shotsRemaining == 0 && arrows.size() == 0) {
            gameOver = true;
        }

        // Update texts
        scoreText.setString("Score: " + to_string(score));
        scoreText.setPosition(600, 550);
        shotsText.setString("Shots Left: " + to_string(shotsRemaining));
        shotsText.setPosition(20, 550);

        // Draw everything
        window.clear();
        if (!gameOver) {
            window.draw(crossbow);
            for (auto& arrow : arrows) {
                window.draw(arrow);
            }
            for (auto& duck : ducks) {
                window.draw(duck);
            }
            window.draw(scoreText);
            window.draw(shotsText);
        } else {
            window.draw(gameOverText);
        }
        window.display();
    }

    return 0;
}
