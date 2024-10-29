#include "Level.h"

//initialisation du constructeur en d�finissant le nb de tile et la position du point cible au millieu du terrain
Level::Level(sf::RenderWindow* window, int setTileCountX, int setTileCountY) :
    tileCountX(setTileCountX), tileCountY(setTileCountY),
    targetX(setTileCountX / 2), targetY(setTileCountY / 2) {
    //chargement des textures
    textureTileWall = TextureLoader::loadTexture("Tile Wall.bmp");
    textureTileTarget = TextureLoader::loadTexture("hdv.bmp");
    textureTileEnemySpawner = TextureLoader::loadTexture("tree2.bmp");

    size_t listTilesSize = (size_t)tileCountX * tileCountY;
    listTiles.assign(listTilesSize, Tile{});

    // Ajout d'un point d'apparition des ennemis � chaque coin de la fen�tre.
    int xMax = tileCountX - 1;
    int yMax = tileCountY - 1;
    setTileType(0, 0, TileType::enemySpawner);
    setTileType(xMax, 0, TileType::enemySpawner);
    setTileType(0, yMax, TileType::enemySpawner);
    setTileType(xMax, yMax, TileType::enemySpawner);

    PathFinding();
}



void Level::draw(sf::RenderWindow* window, int tileSize) {
    sf::RectangleShape rect(sf::Vector2f(tileSize, tileSize));//dessine le patron d'une tile
    //dessine toutes les tiles
    for (int y = 0; y < tileCountY; y++) {
        for (int x = 0; x < tileCountX; x++) {
            if ((x + y) % 2 == 0)
                rect.setFillColor(sf::Color(100, 175, 60, 255));
            else
                rect.setFillColor(sf::Color(110, 215, 70, 255));

            rect.setPosition(x * tileSize, y * tileSize);
            window->draw(rect);
        }
    }
    //desssine tous les points d'apparition
    for (int y = 0; y < tileCountY; y++) {
        for (int x = 0; x < tileCountX; x++) {
            if (getTileType(x, y) == TileType::enemySpawner) {
                sf::Sprite sprite(*textureTileEnemySpawner);
                sprite.setPosition(x * tileSize, y * tileSize);
                sprite.setScale((float)tileSize / sprite.getTexture()->getSize().x, (float)tileSize / sprite.getTexture()->getSize().y);
                window->draw(sprite);
            }
        }
    }
    //dessine le point cible
    if (textureTileTarget != nullptr) {
        sf::Sprite sprite(*textureTileTarget);
        sprite.setPosition(targetX * tileSize, targetY * tileSize);
        sprite.setScale((float)tileSize / sprite.getTexture()->getSize().x, (float)tileSize / sprite.getTexture()->getSize().y);
        window->draw(sprite);
    }
    //dessine tous les murs
    for (int y = 0; y < tileCountY; y++) {
        for (int x = 0; x < tileCountX; x++) {
            if (isTileWall(x, y)) {
                sf::Sprite sprite(*textureTileWall);
                sprite.setPosition(
                    x * tileSize + tileSize / 2 - sprite.getTexture()->getSize().x / 2,
                    y * tileSize + tileSize / 2 - sprite.getTexture()->getSize().y / 2);
                window->draw(sprite);
            }
        }
    }
}

Vector2D Level::getRandomLocation() {
    // Cr�er une liste de toutes les tuiles qui sont des g�n�rateurs d'ennemis.
    std::vector<int> listSpawnerIndices;
    for (int count = 0; count < listTiles.size(); count++) {
        auto& tileSelected = listTiles[count];
        if (tileSelected.type == TileType::enemySpawner)
            listSpawnerIndices.push_back(count);
    }

    // Si un ou plusieurs g�n�rateurs sont trouv�s, en choisir un au hasard et retourner sa position centrale.
    if (!listSpawnerIndices.empty()) {
        int index = listSpawnerIndices[rand() % listSpawnerIndices.size()];
        return Vector2D(static_cast<float>(index % tileCountX) + 0.5f, static_cast<float>(index / tileCountX) + 0.5f);
    }

    return Vector2D(0.5f, 0.5f);
}

//retourne une valeur vrai ou fausse si la tile s�l�ctionner est un mur
bool Level::isTileWall(int x, int y) {
    return (getTileType(x, y) == TileType::wall);
}

//cr�er une nouvelle tile en mettant le type de la tile � l'aide de la fonction setTile type()
void Level::setTileWall(int x, int y, bool setWall) {
    if (getTileType(x, y) != TileType::enemySpawner)//v�rifier qu'il n'y ait pas de point d'apparition des ennemies
        setTileType(x, y, (setWall ? TileType::wall : TileType::empty));
}

//renvoyer le type de la tile
Level::TileType Level::getTileType(int x, int y) {
    int index = x + y * tileCountX;//d�finir l'index de la tile s�l�ctionner
    if (index > -1 && index < listTiles.size() &&
        x > -1 && x < tileCountX &&
        y > -1 && y < tileCountY)
        return listTiles[index].type;

    return TileType::empty;
}

//changer le type d'une tile
void Level::setTileType(int x, int y, TileType tileType) {
    int index = x + y * tileCountX;
    if (index > -1 && index < listTiles.size() &&
        x > -1 && x < tileCountX &&
        y > -1 && y < tileCountY) {
        listTiles[index].type = tileType;
        PathFinding();
    }
}

//renvoie la position du point cible
Vector2D Level::getCiblePos() {
    return Vector2D((float)targetX + 0.5f, (float)targetY + 0.5f);
}


void Level::PathFinding() {
    //d�finir la tile du point cible
    int indexTarget = targetX + targetY * tileCountX;
    //regarde si le point cible est dans le territoire
    if (indexTarget > -1 && indexTarget < listTiles.size() &&
        targetX > -1 && targetX < tileCountX &&
        targetY > -1 && targetY < tileCountY) {

        //Reinitialise les donn�es de flux de chaque tiles, chaque tiles se trouve � une distance 255 du point cible et leur direction x et y sont � z�ro
        for (auto& tileSelected : listTiles) {
            tileSelected.flowDirectionX = 0;
            tileSelected.flowDirectionY = 0;
            tileSelected.flowDistance = flowDistanceMax;
        }

        //Calcule le Pathfinding.
        calculeDistance();
        PathDirection();
    }
}

//Calcule la position de chaque tiles au point cible
void Level::calculeDistance() {
    //d�finir la tile du point cible
    int indexTarget = targetX + targetY * tileCountX;
    //Cr�e une liste qui contient tout les index � v�rifier
    std::queue<int> listIndicesToCheck;
    //Mettre la distance de la tile du point cible � z�ro
    listTiles[indexTarget].flowDistance = 0;
    listIndicesToCheck.push(indexTarget);

    //Cr�er une deuxi�me liste qui contient la distance des 4 tiles voisines
    //exemple: tile � droite du point cible dois parcourir 0 y et 1 x pour arriver au point cible
    const int listNeighbors[][2] = { { -1, 0}, {1, 0}, {0, -1}, {0, 1} };

    //cr�er une boucle qui regarde dans la liste et assigne une distance � chaque tile tant que tous les indices de toutes les tiles ne sont pas v�rifi�
    while (listIndicesToCheck.empty() == false) {
        int indexCurrent = listIndicesToCheck.front();//definir l'index � v�rifier comme le premier index de la liste "listIndicesToCheck"
        listIndicesToCheck.pop();//supprime le dernier index, v�rifi� pr�c�dement

        //regarder chaque tiles voisines (les voisins des voisins des voisins...)
        for (int count = 0; count < 4; count++) {
            int neighborX = listNeighbors[count][0] + indexCurrent % tileCountX;
            int neighborY = listNeighbors[count][1] + indexCurrent / tileCountX;
            int indexNeighbor = neighborX + neighborY * tileCountX;

            //v�rifier la tile voisine existe dans la map et qu'elle n'est pas un mur
            if (indexNeighbor > -1 && indexNeighbor < listTiles.size() &&
                neighborX > -1 && neighborX < tileCountX &&
                neighborY > -1 && neighborY < tileCountY &&
                listTiles[indexNeighbor].type != TileType::wall) {

                //regarder si la tile voisine � d�j� une distance assign� ou pas
                if (listTiles[indexNeighbor].flowDistance == flowDistanceMax) {
                    //ajouter la distance(distance de l'index pr�c�dent +1) � la liste et passer au prochain voisin
                    listTiles[indexNeighbor].flowDistance = listTiles[indexCurrent].flowDistance + 1;
                    listIndicesToCheck.push(indexNeighbor);//ajouter l'index voisin � la liste des indices � v�rifier
                }
            }
        }
    }
}

//Donne une direction x et y � chaque tile pour qu'elle pointe vers le point cible
void Level::PathDirection() {
    //cr�er une nouvelle liste des 8 tiles voisines, prends aussi les tiles en diagonales
    //(ex: la tile en haut � droite est a +1 y et +1 x de la tile s�l�ctionn�
    const int listNeighbors[][2] = {
        {-1, 0}, {-1, 1}, {0, 1}, {1, 1},
        {1, 0}, {1, -1}, {0, -1}, {-1, -1} };

    //s�l�ctionner toutes les tiles une par une
    for (int indexCurrent = 0; indexCurrent < listTiles.size(); indexCurrent++) {
        //V�rifie que la tile s�l�ctionner a bien une distance assign� (grace � la fonction calculeDistance()) 
        // et autre que flowDistanceMax mis par d�faut dans la classe Level.
        if (listTiles[indexCurrent].flowDistance != flowDistanceMax) {
            //initialiser la meilleur distance � la distance de la tiles(au cas ou la tile selectionner est le point cible)
            unsigned char flowFieldBest = listTiles[indexCurrent].flowDistance;

            //regarder chacun des voisins de la tiles s�l�ctionn�(diagonales comprises).
            for (int count = 0; count < 8; count++) {
                int offsetX = listNeighbors[count][0];
                int offsetY = listNeighbors[count][1];
                //calcule les coordonn�es de la tile voisine
                int neighborX = offsetX + indexCurrent % tileCountX;
                int neighborY = offsetY + indexCurrent / tileCountX;
                int indexNeighbor = neighborX + neighborY * tileCountX;

                //v�rifie que le voisin existe.
                if (indexNeighbor > -1 && indexNeighbor < listTiles.size() &&
                    neighborX > -1 && neighborX < tileCountX &&
                    neighborY > -1 && neighborY < tileCountY) {
                    // si le voisin s�l�ctionner � une plus petite distance que la tile s�l�ctionner, met � jour la meilleur distance
                    if (listTiles[indexNeighbor].flowDistance < flowFieldBest) {
                        flowFieldBest = listTiles[indexNeighbor].flowDistance;
                        //pointer les directions x et y de la tile s�l�ctionner en direction de ce voisin
                        listTiles[indexCurrent].flowDirectionX = offsetX;
                        listTiles[indexCurrent].flowDirectionY = offsetY;
                    }
                }
            }
        }
    }
}


//renvoie la direction sous forme de vecteur d'une tile en position (x,y)
Vector2D Level::getNormalPath(int x, int y) {
    int index = x + y * tileCountX;//def index tile en position x,y
    if (index > -1 && index < listTiles.size() &&
        x > -1 && x < tileCountX &&
        y > -1 && y < tileCountY)//v�rifie existance
        return Vector2D((float)listTiles[index].flowDirectionX, (float)listTiles[index].flowDirectionY).normalize();//return directions en vecteur

    return Vector2D();//renvoyer un vecteur nul pour pas faire planter programme si mur ou autre
}
