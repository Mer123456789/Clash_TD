#include "Game.h"
#include <iostream>

// Initialisation du constructeur
//Mode de placement sur wall par défaut
//création d'un nouveau niveau -> si le pointer window n'est pas nul, créer le niveau. sinon envoyer un pointeur nul.
Game::Game(sf::RenderWindow* window, int windowWidth, int windowHeight)
    : placementModeCurrent(PlacementMode::wall),
    level(window != nullptr ? window : nullptr, int(windowWidth / tileSize), int(windowHeight / tileSize)),
    spawnTimer(0.25f), roundTimer(5.0f) {

    //Lancer le jeu
    if (window != nullptr) {

        //Chargement du son de l'apparition d'une unité.
        soundBufferSpawnUnit = *SoundLoader::loadSound("Spawn Unit.mp3");

        //chargement de la police
        if (!font1.loadFromFile("Data/font/LibreBaskerville-Bold.ttf")) {
            std::cerr << "No font is here";
        }

        //contient le temps du jeu
        auto time1 = std::chrono::system_clock::now();
        auto time2 = std::chrono::system_clock::now();

        //Definir le temps entre chaque frame (définission des fps, 60fps)
        const float dT = 1.0f / 60.0f;

        //Commencer la boucle du jeu
        bool running = true;
        while (running) {
            //Determiner le temps écoulé entre chaque frame
            time2 = std::chrono::system_clock::now();
            std::chrono::duration<float> timeDelta = time2 - time1;
            float timeDeltaFloat = timeDelta.count();

            //Si assez de temps à passer, généerer la prochaine frame
            if (timeDeltaFloat >= dT) {
                //contient le nouveau temps pour la prochaine frame
                time1 = time2;

                manageEvents(*window, running);
                update(*window, dT);
                draw(*window);
                if (myHealth <= 0) {
                    running = false;
                }
            }
        }
    }
}

// Destructeur
Game::~Game() {
    TextureLoader::deallocateTextures();
    SoundLoader::deallocateSounds();
}

// Gérer les evenements
void Game::manageEvents(sf::RenderWindow& window, bool& running) {
    bool mouseDownThisFrame = false;

    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            running = false;
            break;

        case sf::Event::MouseButtonPressed:
            mouseDownThisFrame = (mouseDownStatus == 0);
            if (event.mouseButton.button == sf::Mouse::Left)
                mouseDownStatus = sf::Mouse::Left;
            else if (event.mouseButton.button == sf::Mouse::Right)
                mouseDownStatus = sf::Mouse::Right;
            break;

        case sf::Event::MouseButtonReleased:
            mouseDownStatus = 0;
            break;

        case sf::Event::KeyPressed:
            switch (event.key.code) {
            case sf::Keyboard::Escape:
                running = false;
                break;

            case sf::Keyboard::Num1:
                placementModeCurrent = PlacementMode::wall;
                break;
            case sf::Keyboard::Num2:
                placementModeCurrent = PlacementMode::turret;
                break;
            }
        }
    }

    // Gérer la position de la souris par rapport à la taille des tiles -> permet de séléctionner une tile spécifique
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    Vector2D posMouse(static_cast<float>(mousePos.x) / tileSize, static_cast<float>(mousePos.y) / tileSize);

    if (mouseDownStatus >= 0) {
        if (mouseDownThisFrame) {
            switch (mouseDownStatus) {
            case sf::Mouse::Left:
                switch (placementModeCurrent) {
                case PlacementMode::wall:
                    if (gold >= costWall) {//vérifie si assez de ressource
                        gold -= costWall;//déduis le cout necessaire
                        level.setTileWall((int)posMouse.x, (int)posMouse.y, true);
                    }
                    break;
                case PlacementMode::turret:
                    if (gold >= costTurret) {
                        gold -= costTurret;
                        addTurret(window, posMouse);
                    }
                    break;
                }
                break;

            case sf::Mouse::Right:
                level.setTileWall(static_cast<int>(posMouse.x), static_cast<int>(posMouse.y), false);
                eraseTurret(posMouse);
                break;
            }
        }
    }
}

// fonction de mise à jour de chaque élément du jeu
void Game::update(sf::RenderWindow& window, float dT) {
    updateUnits(dT);//mise à jour des unités

    for (auto& turretSelected : listTurrets)
        turretSelected.update(window, dT, listUnits, listProjectiles);//mise à jour des tours

    updateProjectiles(dT);//mise à jour des projectiles
    Waves(window, dT);//apparition de nouvelles unités si besoin
}

//mise à jour des unités
void Game::updateUnits(float dT) {
    //prends toutes les unités contenu dans listUnits et toutes les mettre à jour
    auto it = listUnits.begin();
    while (it != listUnits.end()) {
        bool increment = true;

        if ((*it) != nullptr) {
            (*it)->update(dT, level, listUnits);

            //regarde si l'unité est encore en vie, sinon l'effacer de la liste et mettre la variable increment sur faux
            if ((*it)->isAlive() == false) {
                if ((*it)->getCible() == true) {
                    myHealth -= int((*it)->getDammage(setDammageBuff));//enlève mes pv
                }
                else { gold += goldUnit; }//gain d'or

                it = listUnits.erase(it);//efface l'unité
                increment = false;
            }
        }

        if (increment)
            it++;
    }
}

//mise à jour des projectiles
void Game::updateProjectiles(float dT) {
    //prends tous les projectiles contenu dans listProjectiles et tous les mettre à jour
    auto it = listProjectiles.begin();
    while (it != listProjectiles.end()) {
        (*it).update(dT, listUnits);

        //regarder si le projectile à toucher quelque chose ou non, et l'effacer si besoin
        if ((*it).getCollisionOccurred())
            it = listProjectiles.erase(it);
        else
            it++;
    }
}

//apparition de nouvelles unités si besoin
void Game::Waves(sf::RenderWindow& window, float dT) {
    spawnTimer.countDown(dT);
    if (listUnits.empty() && spawnUnitCount == 0) {
        roundTimer.countDown(dT);
        if (roundTimer.timeSIsZero()) {
            //définis le nombre d'unité devant apparaitre à la prochaine vague
            waves += 1;
            if (waves <= 25) { spawnUnitCount = 10 * waves; }
            else { spawnUnitCount = 250; }
            if (myHealth < maxHealth) {
                myHealth += HealthWaves;
                if (myHealth >= maxHealth) {
                    myHealth = maxHealth;
                }
            }
            getRandomBuff();
            roundTimer.resetToMax();
        }
    }
    if (spawnUnitCount > 0 && spawnTimer.timeSIsZero()) {
        // Jouer le son d'apparition de chaque unités
        soundSpawnUnit.setBuffer(soundBufferSpawnUnit);
        soundSpawnUnit.play();

        addUnit(window, level.getRandomLocation());

        spawnUnitCount--;
        spawnTimer.resetToMax();
    }
}
//crée des buff ou des débuffs aléatoire
void Game::getRandomBuff() {
    //une chance sur trois d'avoir: un buff, debuff ou rien.
    buff = int(rand() % 18);//définis le buff ou debuff (nb entre 0 et 17)

    //buff
    if (buff < 6) {
        buffMessage.setFillColor(sf::Color::Green);
        //+10 point de vie (juste pour un tour)
        if (buff == 0) {
            myHealth += HealthWaves * 2;
            buffMessage.setString("+10 point de vie.");
        }
        //-1 dégats ennemies (permanent)
        if (buff == 1) {
            setDammageBuff -= 1;
            buffMessage.setString("-1 dégats par ennemis.");
        }
        //-0.1 vitesse rechargement = meilleur réaction (permanent)
        if (buff == 2) {
            if (tW > 0.2) {
                tW -= 0.1f;
                buffMessage.setString("-0.1 vitesse rechargement.");
            }
            else {
                buffMessage.setFillColor(sf::Color::Black);
                buffMessage.setString("Aucun buff n'a été assigné.");
            }
        }
        //+2.5 Projectile speed = meilleur visé (permanent)
        if (buff == 3) {
            speedProjectile += 2.5f;
            buffMessage.setString("+2.5 vitesse des projectiles.");
        }
        //+1 Projectile damage (permanent)
        if (buff == 4) {
            damageUnit += 1;
            buffMessage.setString("+1 dégats des tours.");
        }
        //+1 Turret range (permanent)
        if (buff == 5) {
            range += 1.0f;
            buffMessage.setString("+1 porté des tours.");
        }
    }
    //debuff
    if (buff > 11) {
        buffMessage.setFillColor(sf::Color::Red);
        //-10 point de vie (juste pour un tour)
        if (buff == 12) {
            myHealth -= HealthWaves * 2;
            buffMessage.setString("-10 point de vie.");
        }
        //+1 dégats ennemies (permanent)
        if (buff == 13) {
            setDammageBuff += 1;
            buffMessage.setString("+1 dégats par ennemis.");
        }
        //+0.1 vitesse rechargement = meilleur réaction (permanent)
        if (buff == 14) {
            tW -= 0.1f;
            buffMessage.setString("+0.1 vitesse rechargement.");
        }
        //-2.5 Projectile speed = meilleur visé (permanent)
        if (buff == 15) {
            if (speedProjectile > 2.5) {
                speedProjectile -= 2.5f;
                buffMessage.setString("-2.5 vitesse des projectiles.");
            }
            else {
                buffMessage.setFillColor(sf::Color::Black);
                buffMessage.setString("Aucun buff n'a été assigné.");
            }
        }
        //-1 turret damage (permanent)
        if (buff == 16) {
            if (damageUnit > 1) {
                damageUnit -= 1;
                buffMessage.setString("-1 dégats des tours.");
            }
            else {
                buffMessage.setFillColor(sf::Color::Black);
                buffMessage.setString("Aucun buff n'a été assigné.");
            }
        }
        //-1 Turret range (permanent)
        if (buff == 17) {
            if (range > 2) {
                range -= 1.0f;
                buffMessage.setString("-1 porté des tours.");
            }
            else {
                buffMessage.setFillColor(sf::Color::Black);
                buffMessage.setString("Aucun buff n'a été assigné.");
            }
        }
    }
    //rien
    else {
        buffMessage.setFillColor(sf::Color::Black);
        buffMessage.setString("Aucun buff n'a été assigné.");
    }
}
// fais apparaitre une nouvelle unité
void Game::addUnit(sf::RenderWindow& window, Vector2D posMouse) {
    listUnits.push_back(std::make_shared<Unit>(&window, posMouse, waves));
}

// dessine touts les élément du jeu
void Game::draw(sf::RenderWindow& window) {
    window.clear(sf::Color::White);

    level.draw(&window, tileSize);

    for (auto& unitSelected : listUnits) {
        if (unitSelected != nullptr)
            unitSelected->draw(&window, tileSize);
    }

    for (auto& turretSelected : listTurrets)
        turretSelected.draw(window, tileSize);

    for (auto& projectileSelected : listProjectiles)
        projectileSelected.draw(&window, tileSize);

    //afficher la barre de point de vie
    pvX = int(window.getSize().x / 2 - pv1Width / 2);
    pvY = 50;

    //contour
    pv1.setSize(sf::Vector2f(pv1Width, 25)); // Taille
    pv1.setPosition(float(pvX), float(pvY));//position
    pv1.setOutlineColor(sf::Color(0, 0, 0)); //Contour
    pv1.setOutlineThickness(float(5)); //Taille du contour
    window.draw(pv1);

    pv2.setSize(sf::Vector2f(pv2Width, 25));
    pv2.setPosition(float(pvX), float(pvY));
    HealthPourCent = int((float(myHealth) / float(maxHealth)) * 100);
    pv2Width = int(HealthPourCent * pv1Width / 100);//modifier la largeur en fonction du nombre de pv restant
    pv2.setFillColor(sf::Color(0, 0, 255));//gérer problème, si erreur->pv=bleu

    //changer la couleur des point de vie en fonction du nombre de point de vie restant
    if (HealthPourCent >= 50) {
        pv2.setFillColor(sf::Color(255, 0, 0)); //couleur intérieur vert
    }
    else if (25 < HealthPourCent && HealthPourCent < 50) {
        pv2.setFillColor(sf::Color(127, 127, 0));//jaune
    }
    else if (HealthPourCent <= 25) {
        pv2.setFillColor(sf::Color(0, 255, 0));//rouge
    }
    window.draw(pv2);

    buffMessage.setFont(font1);
    buffMessage.setCharacterSize(size_buff);
    buffMessage.setStyle(sf::Text::Bold);
    buffMessage.setPosition(100, 10);

    window.draw(buffMessage);

    sf::String goldText;

    goldMessage.setFont(font1);
    goldMessage.setFillColor(sf::Color::Yellow);
    goldMessage.setString(std::to_string(gold));
    goldMessage.setCharacterSize(size_gold);
    goldMessage.setStyle(sf::Text::Bold);
    goldMessage.setPosition(window.getSize().x - 200, 10);

    window.draw(goldMessage);

    window.display();
}

// ajoute une nouvelle tours à la même position que celle de la souris
void Game::addTurret(sf::RenderWindow& window, Vector2D posMouse) {
    Vector2D pos(float(static_cast<int>(posMouse.x) + 0.5f), float(static_cast<int>(posMouse.y) + 0.5f));
    listTurrets.push_back(Turret(window, pos, tW, range, speedProjectile, damageUnit));
}

// Efface toutes les tours qui se trouvent à la même position que celle de la souris
void Game::eraseTurret(Vector2D posMouse) {
    for (auto it = listTurrets.begin(); it != listTurrets.end();) {
        if ((*it).checkOnTile(static_cast<int>(posMouse.x), static_cast<int>(posMouse.y))) {
            it = listTurrets.erase(it);
        }
        else {
            ++it;
        }
    }
}
