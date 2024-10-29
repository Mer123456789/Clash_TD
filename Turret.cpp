#include "Turret.h"

//définis la vittesse angulaire en radian et la porté de la tour
const float Turret::speedAngular = MathAddon::angleDegToRad(180.0f);

//initialise la position, l'angle, le temps de rechargement, le son et les textures d'une tour
Turret::Turret(sf::RenderWindow& window, Vector2D setPos, float tW, float range, float speed, int damage) :
    pos(setPos), angle(0.0f), timerWeapon(tW), weaponRange(range), speedProjectile(speed), damageUnit(damage) {
    textureMain = TextureLoader::loadTexture("canon.bmp");
    textureSupport = TextureLoader::loadTexture("support.bmp");
    sf::SoundBuffer* soundBufferShoot = SoundLoader::loadSound("Turret Shoot.mp3");
    soundShoot.setBuffer(*soundBufferShoot);
}

void Turret::update(sf::RenderWindow& window, float dT, std::vector<std::shared_ptr<Unit>>& listUnits,
    std::vector<Projectile>& listProjectiles) {
    // met à jour le temps de rechargement
    timerWeapon.countDown(dT);

    // vérifier si l'unité ciblé est invalide (hors de porté ou morte)
    if (auto unitTargetSP = unitTarget.lock()) {
        if (!unitTargetSP->isAlive() || (unitTargetSP->getPos() - pos).magnitude() > weaponRange) {
            //stop avec cette cible
            unitTarget.reset();
        }
    }

    //chercher une nouvelle cible si besoin
    if (unitTarget.expired()) {
        unitTarget = findEnemyUnit(listUnits);
    }

    //met à jour l'angle et tire un projectile
    if (updateAngle(dT)) {
        shootProjectile(window, listProjectiles);
    }
}

// met à jour l'angle 
bool Turret::updateAngle(float dT) {
    //tourne vers l'unité ciblée
    if (auto unitTargetSP = unitTarget.lock()) {
        //Calcule un vecteur normalisé pointant de la tourelle vers la cible
        Vector2D directionNormalTarget = (unitTargetSP->getPos() - pos).normalize();
        //Calcul de l'angle vers la cible
        float angleToTarget = directionNormalTarget.angleBetween(Vector2D(angle));

        //Calcul de l'angle de rotation 
        float angleMove = -copysign(speedAngular * dT, angleToTarget);
        //Si l'angle de rotation calculé est plus grand que l'angle nécessaire pour atteindre la cible,
        // alors la tour s'aligne directement sur la cible.
        if (abs(angleMove) > abs(angleToTarget)) {
            angle = directionNormalTarget.angle();
            return true;//si tour aligné
        }
        else {
            //la tourelle tourne partiellement vers la cible
            angle += angleMove;
        }
    }
    return false;//si dois continuer de bouger
}


void Turret::shootProjectile(sf::RenderWindow& window, std::vector<Projectile>& listProjectiles) {
    if (timerWeapon.timeSIsZero()) {
        listProjectiles.push_back(Projectile(&window, pos, Vector2D(angle), speedProjectile, damageUnit));

        // jouer le son d'un projectile tiré
        soundShoot.play();
        //remettre le temps de chargement au max
        timerWeapon.resetToMax();
    }
}
//dessine la tour dans une tile donné
void Turret::draw(sf::RenderWindow& window, int tileSize) {
    sf::Sprite spriteSupport(*textureSupport);
    spriteSupport.setPosition(
        (pos.x * tileSize) - spriteSupport.getGlobalBounds().width / 2 + offset,
        (pos.y * tileSize) - spriteSupport.getGlobalBounds().height / 2 + offset
    );
    window.draw(spriteSupport);

    sf::Sprite spriteMain(*textureMain);
    float w = (pos.x * tileSize) - spriteMain.getGlobalBounds().width / 2;
    float h = (pos.y * tileSize) - spriteMain.getGlobalBounds().height / 2;

    spriteMain.setOrigin(sf::Vector2f(spriteMain.getLocalBounds().width, spriteMain.getLocalBounds().height) / 2.f);
    spriteMain.setPosition(pos.x * tileSize, pos.y * tileSize);
    spriteMain.setRotation(MathAddon::angleRadToDeg(angle));

    window.draw(spriteMain);
}

//vérifie si elle est sur la tile de position x et y
bool Turret::checkOnTile(int x, int y) {
    return (int)pos.x == x && (int)pos.y == y;
}

//cherche l'unité la plus proche
std::weak_ptr<Unit> Turret::findEnemyUnit(std::vector<std::shared_ptr<Unit>>& listUnits) {
    std::weak_ptr<Unit> closestUnit;
    float distanceClosest = 0.0f;
    //regarde toutes les unités
    for (auto& unitSelected : listUnits) {
        if (unitSelected) {//assure que le pointeur vers l'unité n'est pas nul
            //calcul distance
            float distanceCurrent = (pos - unitSelected->getPos()).magnitude();
            //si l'unité est à portée de tir 
            // et si c'est la première unité valide trouvée ou si elle est plus proche que l'unité précédemment trouvé
            if (distanceCurrent <= weaponRange &&
                (closestUnit.expired() || distanceCurrent < distanceClosest)) {
                closestUnit = unitSelected;
                distanceClosest = distanceCurrent;
            }
        }
    }
    //retourne le weak pointer(vide si sans unité)
    return closestUnit;
}
