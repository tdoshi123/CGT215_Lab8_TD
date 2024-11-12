// Lab8_TD.cpp : This file contains the 'main' function. Program execution begins and ends there. //

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFPhysics/PhysicsShapeList.hpp>
#include <iostream>
#include <vector>

// Constants for game
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int CROSSBOW_X = 100;
const int DUCK_SPEED = 5;
const int MAX_SHOTS = 5;

class DuckHunterGame {
private:
    sf::RenderWindow window;
    sf::Texture crossbowTexture;
    sf::Sprite crossbowSprite;
    sf::Texture duckTexture;
    sf::Font font;
    sf::Text scoreText;
    sf::Text shotsText;
    int score = 0;
    int shotsLeft = MAX_SHOTS;
    sfPhysics::PhysicsShapeList ducks; // List of duck objects
    
public:
    DuckHunterGame() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Duck Hunter") {
        // Load assets
        if (!crossbowTexture.loadFromFile("path/to/crossbow.png") || 
            !duckTexture.loadFromFile("path/to/duck.png") || 
            !font.loadFromFile("path/to/font.ttf")) {
            std::cerr << "Error loading assets\n";
            return;
        }

        // Set up crossbow
        crossbowSprite.setTexture(crossbowTexture);
        crossbowSprite.setPosition(CROSSBOW_X, WINDOW_HEIGHT - crossbowTexture.getSize().y - 50);
        
        // Set up score display
        scoreText.setFont(font);
        scoreText.setPosition(WINDOW_WIDTH - 100, WINDOW_HEIGHT - 50);
        scoreText.setString("Score: 0");

        // Set up shots display
        shotsText.setFont(font);
        shotsText.setPosition(10, WINDOW_HEIGHT - 50);
        shotsText.setString("Shots: " + std::to_string(shotsLeft));
    }

    void run() {
        sf::Clock duckSpawnClock;  // To control duck spawning
        while (window.isOpen()) {
            handleEvents();
            if (duckSpawnClock.getElapsedTime().asMilliseconds() > 1000) { // spawn duck every second
                spawnDuck();
                duckSpawnClock.restart();
            }
            update();
            render();
        }
    }

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                if (shotsLeft > 0) {
                    shoot();
                    shotsLeft--;
                    shotsText.setString("Shots: " + std::to_string(shotsLeft));
                }
            }
        }
    }

    void spawnDuck() {
        sf::RectangleShape duck(sf::Vector2f(50, 30));
        duck.setTexture(&duckTexture);
        duck.setPosition(-50, rand() % (WINDOW_HEIGHT / 2));  // Spawn offscreen, random y
        ducks.add(duck);  // Add duck to the PhysicsShapeList
    }

    void update() {
        // Move ducks and check for collisions
        for (auto& duck : ducks) {
            duck.move(DUCK_SPEED, 0);
            if (duck.getPosition().x > WINDOW_WIDTH) {
                ducks.remove(duck); // Remove duck if it reaches the end
            }
            // Check collision with shot (crossbow) - placeholder logic
            // Add scoring and removal on collision
        }
    }

    void shoot() {
        // Logic to shoot arrow (placeholder) - to implement arrow movement and collision detection
        // Increase score on successful hit and update scoreText
    }

    void render() {
        window.clear();
        window.draw(crossbowSprite);
        for (const auto& duck : ducks) {
            window.draw(duck);
        }
        window.draw(scoreText);
        window.draw(shotsText);
        window.display();
    }
};

int main() {
    DuckHunterGame game;
    game.run();
    return 0;
}
