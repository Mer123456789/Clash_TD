#pragma once
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Math.h"
#include "Level.h"
#include "Ressource.h"

class Game;

class Unit
{
public:
    Unit(sf::RenderWindow* window, Vector2D setPos, int waves);
    void update(float dT, Level& level, std::vector<std::shared_ptr<Unit>>& listUnits);
    bool checkCollisionOver(Vector2D posOther, float sizeOther);
    Vector2D getPos();
    bool getCible();
    bool isAlive();
    int getDammage(int setDammageBuff);
    void removeHealth(int damageUnit);
    void draw(sf::RenderWindow* window, int tileSize);

    int damage = 1;//dégats de chaque unitées

    int sprite_nb;

private:
    Vector2D pos;//position
    static float speed;//vitesse
    static const float size;//taille

    sf::Sprite sprite;
    /*
    sf::Texture* texture_top;
    sf::Texture* texture_topRight;
    sf::Texture* texture_topLeft;
    sf::Texture* texture_Left;
    sf::Texture* texture_Right;
    sf::Texture* texture_bottomRight;
    sf::Texture* texture_bottomLeft;
    sf::Texture* texture_bottom;
    sf::Texture* texture_bottom;

    int sprite_nb=1;
    */


    Timer timerJustHurt;//temps de douleur

    int healthMax = 2;//point de vie max d'une unité
    int healthCurrent = healthMax;//point de vie durant le jeu

    const int WavesChange = 5;//def des vagues de changement

    bool hitTarget = false;//conditions si il a touché la cible
};
