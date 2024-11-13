// Lab8_TD.cpp : This file contains the 'main' function. Program execution begins and ends there. //

#include <SFML/Graphics.hpp>
#include <SFPhysics/PhysicsShapeList.h> // Include SFPhysics for physics-based objects
#include <iostream>

// Constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int MAX_SHOTS = 5;
const float DUCK_SPAWN_INTERVAL = 2000.0f; // 2000 ms or 2 seconds

class DuckHunterGame {
public:
    DuckHunterGame();
    void run();

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void spawnDuck();
    void resetGame();

    sf::RenderWindow window;
    sf::Texture crossbowTexture, arrowTexture, duckTexture;
    sf::Sprite crossbowSprite;
    sf::Font font;
    sf::Text scoreText, shotsLeftText, gameOverText;

    sf::Clock clock; // Main game clock
    sf::Time lastTime;
    float duckMS = 0.0f; // Timer for duck spawning
    int score = 0;
    int shotsFired = 0;
    bool gameOver = false;

    sfphysics::PhysicsShapeList<sf::RectangleShape> duckList; // Duck list
    sfphysics::PhysicsShapeList<sf::RectangleShape> arrowList; // Arrow list
};

// Constructor to initialize game resources
DuckHunterGame::DuckHunterGame() : window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Duck Hunter Game") {
    crossbowTexture.loadFromFile("assets/images/crossbow.png");
    arrowTexture.loadFromFile("assets/images/arrow.png");
    duckTexture.loadFromFile("assets/images/duck.png");
    font.loadFromFile("assets/fonts/arial.ttf");

    crossbowSprite.setTexture(crossbowTexture);
    crossbowSprite.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50);

    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    
    shotsLeftText.setFont(font);
    shotsLeftText.setCharacterSize(24);
    shotsLeftText.setFillColor(sf::Color::White);

    gameOverText.setFont(font);
    gameOverText.setCharacterSize(30);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("GAME OVER");
    gameOverText.setPosition(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2);
}

void DuckHunterGame::run() {
    lastTime = clock.restart();
    while (window.isOpen()) {
        processEvents();
        sf::Time deltaTime = clock.restart();
        update(deltaTime);
        render();
    }
}

void DuckHunterGame::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
            if (gameOver) {
                resetGame();
            } else if (shotsFired < MAX_SHOTS) {
                sfphysics::PhysicsShapeRectangle* arrow = new sfphysics::PhysicsShapeRectangle({10, 30});
                arrow->setTexture(&arrowTexture);
                arrow->setPosition(crossbowSprite.getPosition());
                arrow->setVelocity({0, -500});
                arrowList.add(arrow);
                shotsFired++;
                if (shotsFired == MAX_SHOTS) gameOver = true;
            }
        }
    }
}

void DuckHunterGame::update(sf::Time deltaTime) {
    if (gameOver) return;

    float deltaMS = deltaTime.asMilliseconds();
    duckMS += deltaMS;

    if (duckMS >= DUCK_SPAWN_INTERVAL) {
        duckMS = 0;
        spawnDuck();
    }

    arrowList.update(deltaMS / 1000.0f);
    duckList.update(deltaMS / 1000.0f);

    // Collision detection between arrows and ducks
    for (auto* arrow : arrowList) {
        for (auto* duck : duckList) {
            if (arrow->getGlobalBounds().intersects(duck->getGlobalBounds())) {
                arrowList.remove(arrow);
                duckList.remove(duck);
                score++;
                break;
            }
        }
    }

    // Remove ducks that pass the screen
    for (auto* duck : duckList) {
        if (duck->getPosition().x > SCREEN_WIDTH) {
            duckList.remove(duck);
        }
    }

    // Update text
    scoreText.setString("Score: " + std::to_string(score));
    shotsLeftText.setString("Arrows left: " + std::to_string(MAX_SHOTS - shotsFired));
    scoreText.setPosition(SCREEN_WIDTH - 120, 10);
    shotsLeftText.setPosition(10, 10);
}

void DuckHunterGame::render() {
    window.clear();
    window.draw(crossbowSprite);
    window.draw(scoreText);
    window.draw(shotsLeftText);

    for (auto* arrow : arrowList) window.draw(*arrow);
    for (auto* duck : duckList) window.draw(*duck);

    if (gameOver) window.draw(gameOverText);

    window.display();
}

void DuckHunterGame::spawnDuck() {
    sfphysics::PhysicsShapeRectangle* duck = new sfphysics::PhysicsShapeRectangle({50, 50});
    duck->setTexture(&duckTexture);
    duck->setPosition(-50, 100 + rand() % (SCREEN_HEIGHT - 200));
    duck->setVelocity({100, 0});  // Move rightward
    duckList.add(duck);
}

void DuckHunterGame::resetGame() {
    shotsFired = 0;
    score = 0;
    gameOver = false;
    arrowList.clear();
    duckList.clear();
}

int main() {
    DuckHunterGame game;
    game.run();
    return 0;
}
