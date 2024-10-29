#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Math.h"
#include "Ressource.h"
#include "Unit.h"

class Projectile
{
public:
    Projectile(sf::RenderWindow* window, Vector2D setPos, Vector2D setDirectionNormal, float vitesse, int damage);
    void update(float dT, std::vector<std::shared_ptr<Unit>>& listUnits);
    void draw(sf::RenderWindow* window, int tileSize);
    bool getCollisionOccurred();

private:
    void checkCollisions(std::vector<std::shared_ptr<Unit>>& listUnits);

    Vector2D pos, directionNormal;//direction et position du projectile
    static const float size, distanceTraveledMax;//taille, vitesse et distance maximum à parcourir
    float speed;
    float distanceTraveled = 0.0f;//distance parcourue

    int damageUnit;//def dégats aux unités

    sf::Texture* texture;
    sf::RectangleShape shape;

    bool collisionOccurred = false;//condition si une collision a eut lieu
};


