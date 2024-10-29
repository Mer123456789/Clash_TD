#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Math.h"
#include "Ressource.h"
#include "Unit.h"
#include "Projectile.h"

class Turret {
public:
    Turret(sf::RenderWindow& window, Vector2D setPos, float tW, float range, float speed, int damage);

    void update(sf::RenderWindow& window, float dT, std::vector<std::shared_ptr<Unit>>& listUnits, std::vector<Projectile>& listProjectiles);
    bool checkOnTile(int x, int y);
    void draw(sf::RenderWindow& window, int tileSize);

private:
    bool updateAngle(float dT);
    void shootProjectile(sf::RenderWindow& window, std::vector<Projectile>& listProjectiles);
    std::weak_ptr<Unit> findEnemyUnit(std::vector<std::shared_ptr<Unit>>& listUnits);

    Vector2D pos;//position
    float angle;//angle de la tour
    static const float speedAngular;
    float weaponRange;
    float speedProjectile;
    int damageUnit;

    Timer timerWeapon;
    std::weak_ptr<Unit> unitTarget;

    sf::Texture* textureMain;
    sf::Texture* textureSupport;
    sf::Sprite spriteMain, spriteSupport;
    int offset = 5;//léger décalage entre la tour et son support

    sf::SoundBuffer soundBufferShoot;
    sf::Sound soundShoot;
};

