#include "Projectile.h"

const float Projectile::size = 0.2f;
const float Projectile::distanceTraveledMax = 20.0f;

//Le constructeur initialise la possition, la direction et la texture du projectile.
Projectile::Projectile(sf::RenderWindow* window, Vector2D setPos, Vector2D setDirectionNormal, float vitesse, int damage) :
    pos(setPos), directionNormal(setDirectionNormal), speed(vitesse), damageUnit(damage) {
    texture = TextureLoader::loadTexture("Projectile1.png");
}

//met à jour la distance et la collision d'un projectile
void Projectile::update(float dT, std::vector<std::shared_ptr<Unit>>& listUnits) {
    // def distnce que le projectile fait en fonction de sa vitesse
    float distanceMove = speed * dT;
    //change sa position en fonction de sa direction
    pos += directionNormal * distanceMove;
    //def la distance parcourue au totale
    distanceTraveled += distanceMove;
    //check si la distance totale est plus grande que la distance max
    if (distanceTraveled >= distanceTraveledMax) {
        collisionOccurred = true;//condition collision sur vrai
    }
    //vérifie si il y a une collision
    checkCollisions(listUnits);
}

//dessine le projectile à sa position tant qu'il n'y a eu aucune collision
void Projectile::draw(sf::RenderWindow* window, int tileSize) {
    if (!collisionOccurred) {
        sf::Sprite sprite(*texture);
        sprite.setPosition(
            pos.x * tileSize - (sprite.getLocalBounds().width / 2),
            pos.y * tileSize - (sprite.getLocalBounds().height / 2));
        window->draw(sprite);
    }
}

//Cette fonction renvoie la condition de collision, vraix ou faux.
bool Projectile::getCollisionOccurred() {
    return collisionOccurred;
}


//vérifie si il y a une collision
void Projectile::checkCollisions(std::vector<std::shared_ptr<Unit>>& listUnits) {
    if (collisionOccurred == false) {
        for (int count = 0; count < listUnits.size() && collisionOccurred == false; count++) {
            auto& unitSelected = listUnits[count];//def une unité déléctionné
            //vérifie si le projectile a la même position qu'une unité
            if (unitSelected != nullptr && unitSelected->checkCollisionOver(pos, size)) {
                unitSelected->removeHealth(damageUnit);//enlever des dégats aux unités
                collisionOccurred = true;//condition collision sur vrai
            }
        }
    }
}
