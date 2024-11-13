#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;
const float DUCK_SPEED = 0.3;
const int DUCK_SPAWN_INTERVAL_MS = 2000; // Adjust for duck spawn rate

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

void MoveCrossbow(PhysicsSprite& crossbow, int elapsedMS) {
    if (Keyboard::isKeyPressed(Keyboard::Right)) {
        Vector2f newPos(crossbow.getCenter());
        newPos.x += KB_SPEED * elapsedMS;
        crossbow.setCenter(newPos);
    }
    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        Vector2f newPos(crossbow.getCenter());
        newPos.x -= KB_SPEED * elapsedMS;
        crossbow.setCenter(newPos);
    }
}

Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds();
    return Vector2f(r.width, r.height);
}

int main() {
    RenderWindow window(VideoMode(800, 600), "Duck Hunter");
    World world(Vector2f(0, 0));
    int score(0);
    int arrows(5);

    // Load sound and textures
    SoundBuffer popBuffer;
    if (!popBuffer.loadFromFile("assets/sounds/balloonpop.ogg")) {
        cout << "could not load balloonpop.ogg" << endl;
        exit(5);
    }
    Sound popSound;
    popSound.setBuffer(popBuffer);

    Music music;
    if (!music.openFromFile("assets/sounds/circus.ogg")) {
        cout << "Failed to load circus.ogg ";
        exit(6);
    }
    music.play();

    Texture cbowTex, arrowTex, redTex;
    LoadTex(cbowTex, "assets/images/crossbow.png");
    LoadTex(arrowTex, "assets/images/arrow.png");
    LoadTex(redTex, "assets/images/duck.png");

    // Set up crossbow
    PhysicsSprite& crossBow = *new PhysicsSprite();
    crossBow.setTexture(cbowTex);
    crossBow.setCenter(Vector2f(400, 550));
    world.AddPhysicsBody(crossBow);

    // Set up arrow
    PhysicsSprite arrow;
    arrow.setTexture(arrowTex);
    bool drawingArrow(false);

    // Create boundaries
    PhysicsRectangle top, left, right;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    left.setSize(Vector2f(10, 600));
    left.setCenter(Vector2f(5, 300));
    left.setStatic(true);
    world.AddPhysicsBody(left);

    right.setSize(Vector2f(10, 600));
    right.setCenter(Vector2f(795, 300));
    right.setStatic(true);
    world.AddPhysicsBody(right);

    // Duck management
    PhysicsShapeList<PhysicsSprite> ducks;
    sf::Clock clock;
    Time lastDuckSpawn = clock.getElapsedTime();

    top.onCollision = [&drawingArrow, &world, &arrow](PhysicsBodyCollisionResult result) {
        drawingArrow = false;
        world.RemovePhysicsBody(arrow);
    };

    Font fnt;
    if (!fnt.loadFromFile("assets/fonts/arial.ttf")) {
        cout << "Could not load font." << endl;
        exit(3);
    }

    while (window.isOpen()) {
        Time currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastDuckSpawn;

        // Spawn ducks periodically
        if (deltaTime.asMilliseconds() >= DUCK_SPAWN_INTERVAL_MS) {
            lastDuckSpawn = currentTime;

            PhysicsSprite& duck = ducks.Create();
            duck.setTexture(redTex);
            duck.setCenter(Vector2f(-50, rand() % 600)); // Starts off-screen, random Y position
            duck.setVelocity(Vector2f(DUCK_SPEED, 0));   // Moves rightward
            world.AddPhysicsBody(duck);

            // Handle collision with the right boundary (remove the duck when it hits)
            duck.onCollision = [&world, &duck, &ducks, &right](PhysicsBodyCollisionResult result) {
                if (&result.object2 == &right) {
                    world.RemovePhysicsBody(duck);
                    ducks.QueueRemove(duck);
                }
            };
        }

        // Arrow firing logic
        if (Keyboard::isKeyPressed(Keyboard::Space) && !drawingArrow && arrows > 0) {
            drawingArrow = true;
            arrows--;
            arrow.setCenter(crossBow.getCenter());
            arrow.setVelocity(Vector2f(0, -1));
            world.AddPhysicsBody(arrow);
        }

        world.UpdatePhysics(10);

        // Check for duck-arrow collisions
        for (PhysicsShape& duck : ducks) {
            PhysicsSprite* duckSprite = dynamic_cast<PhysicsSprite*>(&duck);
            if (duckSprite && duckSprite->onCollision) {
                duckSprite->onCollision = [&drawingArrow, &world, &arrow, &duck, &ducks, &score, &popSound](PhysicsBodyCollisionResult result) {
                    if (&result.object2 == &arrow) {
                        popSound.play();
                        drawingArrow = false;
                        world.RemovePhysicsBody(arrow);
                        world.RemovePhysicsBody(duck);
                        ducks.QueueRemove(duck);
                        score += 10;
                    }
                };
            }
        }

        window.clear();
        window.draw(crossBow);

        if (drawingArrow) {
            window.draw(arrow);
        }

        ducks.DoRemovals(); // Remove ducks that are queued for removal
        for (PhysicsShape& duck : ducks) {
            window.draw(static_cast<PhysicsSprite&>(duck));
        }

        // Display score and remaining arrows
        Text scoreText, arrowCountText;
        scoreText.setFont(fnt);
        scoreText.setString("Score: " + to_string(score));
        scoreText.setPosition(700, 550);

        arrowCountText.setFont(fnt);
        arrowCountText.setString("Arrows: " + to_string(arrows));
        arrowCountText.setPosition(20, 550);

        window.draw(scoreText);
        window.draw(arrowCountText);
        window.display();
    }

    return 0;
}
