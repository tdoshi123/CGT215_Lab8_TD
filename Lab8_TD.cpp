// Lab8_TD.cpp : This file contains the 'main' function. Program execution begins and ends there. //

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFPhysics/PhysicsShapeList.hpp>
#include <iostream>
#include <vector>

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
    sfPhysics::PhysicsShapeList ducks;

public:
    DuckHunterGame() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Duck Hunter") {
        if (!crossbowTexture.loadFromFile("\\nas01.itap.purdue.edu\puhome\desktop\CGT 215 Files\Lab8_TD\assets\images\crossbow.png") ||
            !duckTexture.loadFromFile("\\nas01.itap.purdue.edu\puhome\desktop\CGT 215 Files\Lab8_TD\assets\images\duck.png") ||
            !font.loadFromFile("\\nas01.itap.purdue.edu\puhome\desktop\CGT 215 Files\Lab8_TD\assets\fonts\arial.ttf")) {
