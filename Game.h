#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <chrono>
#include <memory>
#include "Unit.h"
#include "Turret.h"
#include "Projectile.h"
#include "Level.h"
#include "Ressource.h"

class Game
{
private:
    // Definition du mode placement, 0=wall, 1=turret
    enum class PlacementMode {
        wall,
        turret
    } placementModeCurrent;

public:
    //fonction du lancement du jeu
    Game(sf::RenderWindow* window, int windowWidth, int windowHeight);
    ~Game();

    int waves = 0;//definis le nombre de vague actuelle

private:
    void manageEvents(sf::RenderWindow& window, bool& running);
    void Waves(sf::RenderWindow& window, float dT);
    void getRandomBuff();
    void update(sf::RenderWindow& window, float dT);
    void updateUnits(float dT);
    void updateProjectiles(float dT);
    void addUnit(sf::RenderWindow& window, Vector2D posMouse);
    void addTurret(sf::RenderWindow& window, Vector2D posMouse);
    void eraseTurret(Vector2D posMouse);
    void draw(sf::RenderWindow& window);

    int mouseDownStatus = -1;//statut de la souris

    const int tileSize = 40;//taille d'une tile

    int myHealth = 50;//sant�, point de vie actuel
    const int maxHealth = 50;//maximum de point de vie � gagn�
    int HealthWaves = 5;//nb de point de vie � recup�r� par vagues
    int HealthPourCent = 50;//le pourcentage de la vie restante

    int gold = 200;//nb or par defaut
    int goldUnit = 2;//nb d'or par enemie tu�

    //cout tour et mur
    const int costTurret = 10;
    const int costWall = 2;

    int buff = 15;//le buff ou debuff donn�
    int setDammageBuff = 0;//buff de dammage donn� au unit�
    float tW = 1.0f;//temps de rechargement
    float range = 5.0f;//port� d'une tour
    float speedProjectile = 10.0f;//vitesse
    int damageUnit = 2;//d�gats des tours au unit�

    //cr�e un nouveau niveau
    Level level;

    //cr�e un vecteur pour les unit�, un vecteur pour les tours et un vecteur pour projectiles
    //les shared_ptr<> permet de garder les references vers les objets, et eviter les copies.
    //Il est ainsi plus facile de controler l'ajout et la suppression d'�l�ment
    std::vector<std::shared_ptr<Unit>> listUnits;
    std::vector<Turret> listTurrets;
    std::vector<Projectile> listProjectiles;

    //definis l'overlay
    sf::Texture textureOverlay;
    sf::Sprite spriteOverlay;
    bool overlayVisible = true;// definir si l'overlay est visible ou pas

    //definis le temps(le temps par apparition et entre chaque round/vague)
    Timer spawnTimer, roundTimer;

    int spawnUnitCount = 0;//nb d'unit� � apparaitre

    //d�finis le son
    sf::SoundBuffer soundBufferSpawnUnit;
    sf::Sound soundSpawnUnit;

    //barre de point de vie
    sf::RectangleShape pv1;
    sf::RectangleShape pv2;

    int pvX = 0;//position x de la barre de point de vie
    int pvY = 0;//position y de la barre de point de vie
    int pv1Width = 250;//largeur de la barre de point de vie
    int pv2Width = 0;//largeur de la barre de point de vie interactive

    sf::Font font1;//d�fini la police
    sf::Text buffMessage;//Message des buff recu
    int size_buff = 25;
    sf::Text goldMessage;//Message de l'or restant
    int size_gold = 25;
};
