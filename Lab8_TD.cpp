// Lab8_TD.cpp : This file contains the 'main' function. Program execution begins and ends there. //

#include <SFML/Graphics.hpp>
#include <SFPhysics/PhysicsShapeList.hpp>
#include <iostream>
#include <vector>

// Constants for game
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int CROSSBOW_X = 100;
const int DUCK_SPEED = 5;
const int ARROW_SPEED = 10;
const int MAX_SHOTS = 5;

class DuckHunterGame {
private:
    sf::RenderWindow window;
    sf::Texture crossbowTexture;
    sf::Sprite crossbowSprite;
    sf::Texture duckTexture;
    sf::Texture arrowTexture;
    sf::Font font;
    sf::Text scoreText;
    sf::Text shotsText;
    int score = 0;
    int shotsLeft = MAX_SHOTS;

    sfPhysics::PhysicsShapeList<sf::RectangleShape> ducks; // List of duck objects
    std::vector<sf::RectangleShape> arrows;                 // List of active arrows

public:
    DuckHunterGame() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Duck Hunter") {
        // Load assets
        if (!crossbowTexture.loadFromFile("\\nas01.itap.purdue.edu\puhome\desktop\CGT 215 Files\Lab8_TD\assets\images\crossbow.png") || 
            !duckTexture.loadFromFile("\\nas01.itap.purdue.edu\puhome\desktop\CGT 215 Files\Lab8_TD\assets\images\duck.png") || 
            !arrowTexture.loadFromFile("\\nas01.itap.purdue.edu\puhome\desktop\CGT 215 Files\Lab8_TD\assets\images\arrow.png") ||
            !font.loadFromFile("\\nas01.itap.purdue.edu\puhome\desktop\CGT 215 Files\Lab8_TD\assets\fonts\arial.ttf")) {
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

    void shoot() {
        sf::RectangleShape arrow(sf::Vector2f(30, 5));  // Arrow size
        arrow.setTexture(&arrowTexture);
        arrow.setPosition(CROSSBOW_X + crossbowSprite.getGlobalBounds().width, crossbowSprite.getPosition().y + crossbowSprite.getGlobalBounds().height / 2);
        arrows.push_back(arrow);  // Add arrow to active arrows list
    }

    void update() {
        // Move ducks and check for collisions with arrows
        for (auto it = ducks.begin(); it != ducks.end(); ) {
            it->move(DUCK_SPEED, 0);
            if (it->getPosition().x > WINDOW_WIDTH) {
                it = ducks.remove(it);  // Remove duck if it reaches the end
            } else {
                ++it;
            }
        }

        // Move arrows and check for collisions with ducks
        for (auto arrowIt = arrows.begin(); arrowIt != arrows.end(); ) {
            arrowIt->move(ARROW_SPEED, 0);
            bool hit = false;
            for (auto duckIt = ducks.begin(); duckIt != ducks.end(); ) {
                if (arrowIt->getGlobalBounds().intersects(duckIt->getGlobalBounds())) {
                    // Collision detected: Remove both duck and arrow
                    duckIt = ducks.remove(duckIt);
                    hit = true;
                    score++;
                    scoreText.setString("Score: " + std::to_string(score));
                    break;  // Break out of duck loop to process next arrow
                } else {
                    ++duckIt;
                }
            }
            if (hit || arrowIt->getPosition().x > WINDOW_WIDTH) {
                arrowIt = arrows.erase(arrowIt);  // Remove arrow if it hit or left the screen
            } else {
                ++arrowIt;
            }
        }
    }

    void render() {
        window.clear();
        window.draw(crossbowSprite);
        for (const auto& duck : ducks) {
            window.draw(duck);
        }
        for (const auto& arrow : arrows) {
            window.draw(arrow);
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
