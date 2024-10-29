#pragma once
#include <queue>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Math.h"
#include "Ressource.h"

class Level
{
private:
    //défini le type d'une tile
    enum class TileType : char {
        empty,
        wall,
        enemySpawner
    };

    //def la distance maximum
    static const unsigned char flowDistanceMax = 255;
    //structure d'une tile avec son type, sa direction et sa distance
    struct Tile {
        TileType type = TileType::empty;
        int flowDirectionX = 0;
        int flowDirectionY = 0;
        unsigned char flowDistance = flowDistanceMax;
    };

public:
    Level(sf::RenderWindow* window, int setTileCountX, int setTileCountY);

    bool isTileWall(int x, int y);
    void setTileWall(int x, int y, bool setWall);

    Vector2D getCiblePos();
    Vector2D getRandomLocation();
    Vector2D getNormalPath(int x, int y);

    void draw(sf::RenderWindow* window, int tileSize);

    //list contenant toutes les tiles
    std::vector<Tile> listTiles;

private:
    TileType getTileType(int x, int y);

    void setTileType(int x, int y, TileType tileType);
    void PathFinding();
    void calculeDistance();
    void PathDirection();

    //dimension des tiles
    const int tileCountX, tileCountY;
    //position du point cible
    const int targetX = 0, targetY = 0;

    sf::Texture* textureTileWall = nullptr;
    sf::Texture* textureTileTarget = nullptr;
    sf::Texture* textureTileEnemySpawner = nullptr;
};
